#include "Buffer.hpp"

#include "VkContext.hpp"
#include "memory/memory.hpp"
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
    VkDeviceSize itemSize,
    std::uint64_t count,
    VkMemoryPropertyFlags memoryPropertyFlags,
    bool mapped)
    : m_count{ count },
      m_usage{ usage },
      m_name{ name },
      m_mapped{ mapped }
{
    assert(itemSize > 0 && "Invalid itemSize");
    assert(count > 0 && "Invalid count");

    if (m_usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
    {
        m_stride = alignUp(
            itemSize,
            VkContext::Device().GetLimits().minUniformBufferOffsetAlignment);
    }
    else if (m_usage & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)
    {
        m_stride = alignUp(
            itemSize,
            VkContext::Device().GetLimits().minStorageBufferOffsetAlignment);
    }
    else
    {
        m_stride = itemSize;
    }

    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.size = m_stride * m_count;
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
            VkContext::Allocator(),
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
            VkContext::LogicalDevice(),
            &deviceAddressInfo);
    }

    VkContext::SetDebugName(VK_OBJECT_TYPE_BUFFER, m_handle, m_name);
}

Buffer::Buffer(
    std::string_view name,
    VkBufferUsageFlags usage,
    VkDeviceSize size,
    VkMemoryPropertyFlags memoryPropertyFlags,
    bool mapped)
    : Buffer(name, usage, size, 1, memoryPropertyFlags, mapped)
{
}

Buffer::Buffer(Buffer&& other) noexcept
    : m_handle{ other.m_handle },
      m_allocation{ other.m_allocation },
      m_info{ other.m_info },
      m_deviceAddress{ other.m_deviceAddress },
      m_stride{ other.m_stride },
      m_count{ other.m_count },
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
        m_stride = other.m_stride;
        m_count = other.m_count;
        m_usage = other.m_usage;
        m_name = other.m_name;
        m_mapped = other.m_mapped;

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

    VkContext::DeletionQueue().AddBuffer(m_handle, m_allocation);
}

void Buffer::Destroy()
{
    vmaDestroyBuffer(VkContext::Allocator(), m_handle, m_allocation);

    m_handle = VK_NULL_HANDLE;
    m_allocation = VK_NULL_HANDLE;
    m_info = {};
    m_deviceAddress = {};
}

void Buffer::Update(const void* data, std::size_t size, std::size_t srcOffset)
{
    assert(m_handle != VK_NULL_HANDLE);
    assert(data != nullptr && "Invalid data");
    assert(size > 0 && (size + srcOffset) <= m_info.size && "Invalid size");

    if (m_mapped)
    {
        memcpy(
            reinterpret_cast<std::byte*>(m_info.pMappedData) + srcOffset,
            data,
            size);
    }
    else
    {
        Buffer staging(
            "Buffer_update_staging",
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            size,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            true);

        staging.Update(data, size);

        VkContext::Device().CmdImmediateSubmit(
            [&](const CommandBuffer& commandBuffer) {
                VkBufferCopy2 regions[1]{
                    VkBufferCopy2{
                        .sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2,
                        .pNext = nullptr,
                        .srcOffset = 0,
                        .dstOffset = srcOffset,
                        .size = staging.GetSize(),
                    },
                };

                commandBuffer.CopyBuffer(
                    staging.GetHandle(),
                    m_handle,
                    regions);
            });

        staging.Destroy();
    }
}

void Buffer::UpdateAt(const void* data, std::size_t size, std::uint64_t index)
{
    assert(size <= m_stride && "Invalid size");
    assert(index < m_count && "Invalid index");

    std::size_t srcOffset{ index * m_stride };

    Update(data, size, srcOffset);
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

VkDeviceSize Buffer::GetStride() const
{
    return m_stride;
}

std::uint64_t Buffer::GetCount() const
{
    return m_count;
}

VkBufferUsageFlags Buffer::GetUsage() const
{
    return m_usage;
}
}
