#include "Buffer.hpp"

#include "VkContext.hpp"
#include "vulkan/vulkan_debug.hpp"

#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cstddef>
#include <string_view>

namespace Zeus
{
Buffer::Buffer(
    std::string_view name,
    VkBufferUsageFlags usage,
    VkDeviceSize size,
    VkMemoryPropertyFlags memoryPropertyFlags,
    bool mapped)
    : m_usage{ usage },
      m_name{ name },
      m_mapped{ mapped }
{
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
        assert(!m_mapped && "Cannot map VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT");
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
        VkBufferDeviceAddressInfo deviceAddressInfo{};
        deviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        deviceAddressInfo.pNext = nullptr;
        deviceAddressInfo.buffer = m_handle;

        m_deviceAddress = vkGetBufferDeviceAddress(
            VkContext::GetLogicalDevice(),
            &deviceAddressInfo);
    }

    VkContext::SetDebugName(VK_OBJECT_TYPE_BUFFER, m_handle, m_name);
}

Buffer::Buffer(Buffer&& other) noexcept
    : m_handle{ other.m_handle },
      m_allocation{ other.m_allocation },
      m_info{ other.m_info },
      m_deviceAddress{ other.m_deviceAddress },
      m_usage{ other.m_usage },
      m_name{ other.m_name },
      m_mapped{ other.m_mapped }
{
    other.m_handle = VK_NULL_HANDLE;
    other.m_allocation = VK_NULL_HANDLE;
    other.m_info = {};
    other.m_deviceAddress = {};
}

Buffer& Buffer::operator=(Buffer&& other)
{
    if (this != &other)
    {
        if (m_handle != VK_NULL_HANDLE)
        {
            Destroy();
        }

        m_handle = other.m_handle;
        m_allocation = other.m_allocation;
        m_info = other.m_info;
        m_deviceAddress = other.m_deviceAddress;
        m_usage = other.m_usage;
        m_mapped = other.m_mapped;
        m_name = other.m_name;

        other.m_handle = VK_NULL_HANDLE;
        other.m_allocation = VK_NULL_HANDLE;
        other.m_info = {};
        other.m_deviceAddress = {};
    }

    return *this;
}

Buffer::~Buffer()
{
    if (m_handle == VK_NULL_HANDLE)
        return;

    VkContext::GetDeletionQueue().AddBuffer(m_handle, m_allocation);
}

void Buffer::Destroy()
{
    vmaDestroyBuffer(VkContext::GetAllocator(), m_handle, m_allocation);

    m_handle = VK_NULL_HANDLE;
    m_allocation = VK_NULL_HANDLE;
    m_info = {};
    m_deviceAddress = {};
}

void Buffer::Update(
    const void* data,
    std::size_t dataSize,
    std::size_t dstOffset)
{
    assert(m_handle != VK_NULL_HANDLE);
    assert(data != nullptr && "Invalid data");
    assert(
        dataSize > 0 && (dataSize + dstOffset) <= m_info.size &&
        "Invalid size");

    if (m_mapped)
    {
        memcpy(
            reinterpret_cast<std::byte*>(m_info.pMappedData) + dstOffset,
            data,
            dataSize);
    }
    else
    {
        Buffer staging(
            "Buffer_update_staging",
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            dataSize,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            true);

        staging.Update(data, dataSize);

        VkContext::GetDevice().CmdImmediateSubmit(
            [&](const CommandBuffer& commandBuffer) {
                commandBuffer.CopyBuffer(
                    staging.GetHandle(),
                    m_handle,
                    staging.GetSize(),
                    0,
                    dstOffset);
            });

        staging.Destroy();
    }
}

void* Buffer::GetData() const
{
    assert(m_mapped);
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

VkBufferUsageFlags Buffer::GetUsage() const
{
    return m_usage;
}
}
