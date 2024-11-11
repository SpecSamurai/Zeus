#include "Buffer.hpp"

#include "Definitions.hpp"
#include "VkContext.hpp"
#include "api/vulkan_buffer.hpp"
#include "api/vulkan_debug.hpp"

#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cstddef>
#include <cstring>

namespace Zeus
{
Buffer::Buffer(
    VkBufferUsageFlags usage,
    VkDeviceSize size,
    VkMemoryPropertyFlags memoryPropertyFlags,
    bool mapped,
    const char* name)
    : m_usage{ usage },
      m_mapped{ mapped }
{
    // set
    // m_type = type;
    // m_stride_unaligned = static_cast<uint32_t>(stride);
    // m_stride = m_stride_unaligned;
    // m_element_count = element_count;
    // m_object_size = stride * element_count;
    // m_mappable = mappable;
    // m_object_name = name;

    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.size = size;
    createInfo.usage = usage;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;

    VmaAllocationCreateInfo allocationCreateInfo{};
    allocationCreateInfo.requiredFlags = memoryPropertyFlags;

    switch (memoryPropertyFlags)
    {
    case VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT:
        allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        break;

    case VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT:
        allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
        break;

    case VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT:
        allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
        break;

    default:
        assert(false && "Unsupported Memory property flag.");
    }

    if (m_mapped)
        allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    VKCHECK(
        vmaCreateBuffer(
            VkContext::GetAllocator(),
            &createInfo,
            &allocationCreateInfo,
            &m_handle,
            &m_allocation,
            &m_info),
        "Failed to create buffer.");

    if (usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
    {
        VkBufferDeviceAddressInfo info{};
        info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        info.pNext = nullptr;
        info.buffer = m_handle;

        m_deviceAddress =
            vkGetBufferDeviceAddress(VkContext::GetLogicalDevice(), &info);
    }

#ifndef NDEBUG
    if (name != nullptr)
    {
        setDebugUtilsObjectNameEXT(
            VkContext::GetLogicalDevice(),
            VK_OBJECT_TYPE_BUFFER,
            reinterpret_cast<std::uint64_t>(m_handle),
            name);
    }
#endif
}

Buffer::~Buffer()
{
    if (m_handle == VK_NULL_HANDLE)
        return;

    VkContext::GetDeletionQueue().Add(ResourceType::Buffer, this);
}

void Buffer::Update(void* data, std::size_t size, std::size_t offset)
{
    assert(data != nullptr && "Invalid data");
    assert(size > 0 && (size + offset) <= m_info.size && "Invalid size");

    if (m_mapped)
    {
        memcpy(
            reinterpret_cast<std::byte*>(m_info.pMappedData) + offset,
            data,
            size);
    }
    else
    {
        Buffer staging(
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            size,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            true);

        staging.Update(data, size);

        VkContext::GetDevice().CmdImmediateSubmit(
            [&](VkCommandBuffer commandBuffer) {
                cmdCopyBuffer(
                    commandBuffer,
                    staging.GetHandle(),
                    m_handle,
                    0,
                    offset,
                    size);
            });

        vmaDestroyBuffer(
            VkContext::GetAllocator(),
            staging.GetHandle(),
            staging.GetAllocation());
    }
}

void Buffer::ImmediateCopyToBuffer(
    const Buffer& dstBuffer,
    std::size_t size,
    std::size_t srcOffset,
    std::size_t dstOffset) const
{
    VkContext::GetDevice().CmdImmediateSubmit(
        [&](VkCommandBuffer commandBuffer) {
            cmdCopyBuffer(
                commandBuffer,
                m_handle,
                dstBuffer.GetHandle(),
                srcOffset,
                dstOffset,
                size);
        });
}

void Buffer::CopyToBuffer(
    VkCommandBuffer commandBuffer,
    const Buffer& dstBuffer,
    std::size_t size,
    std::size_t srcOffset,
    std::size_t dstOffset) const
{
    cmdCopyBuffer(
        commandBuffer,
        m_handle,
        dstBuffer.GetHandle(),
        srcOffset,
        dstOffset,
        size);
}

void Buffer::Destroy()
{
    if (m_handle == VK_NULL_HANDLE)
        return;

    vmaDestroyBuffer(VkContext::GetAllocator(), m_handle, m_allocation);

    m_handle = VK_NULL_HANDLE;
    m_allocation = VK_NULL_HANDLE;
}

void* Buffer::GetData() const
{
    return m_info.pMappedData;
}

VkBuffer Buffer::GetHandle() const
{
    return m_handle;
}

VmaAllocation Buffer::GetAllocation() const
{
    return m_allocation;
}

const VmaAllocationInfo& Buffer::GetInfo() const
{
    return m_info;
}

VkDeviceSize Buffer::GetSize() const
{
    return m_info.size;
}

VkDeviceAddress Buffer::GetDeviceAddress() const
{
    assert(m_usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
    return m_deviceAddress;
}
}
