#pragma once

#include "rhi/vulkan_memory.hpp"

#include <vulkan/vulkan_core.h>

#include <cstddef>

namespace Zeus
{
class Buffer
{
public:
    Buffer(
        VkBufferUsageFlags usage,
        VkDeviceSize size,
        VkMemoryPropertyFlags memoryPropertyFlags,
        bool mapped = false,
        const char* name = nullptr);

    ~Buffer();

    void Update(void* data, std::size_t size, std::size_t srcOffset = 0);
    // void ResetOffset() { m_offset = 0; first_update = true; }
    void ImmediateCopyToBuffer(
        const Buffer& dstBuffer,
        std::size_t size,
        std::size_t srcOffset = 0,
        std::size_t dstOffset = 0) const;

    void CopyToBuffer(
        VkCommandBuffer commandBuffer,
        const Buffer& dstBuffer,
        std::size_t size,
        std::size_t srcOffset = 0,
        std::size_t dstOffset = 0) const;

    void Destroy();

    void* GetData() const;
    VkBuffer GetHandle() const;
    VmaAllocation GetAllocation() const;
    const VmaAllocationInfo& GetInfo() const;
    VkDeviceSize GetSize() const;
    VkDeviceAddress GetDeviceAddress() const;

    // TODO
    // Add minimumAllignment
    // Add offset
    // Add DataCount
    // uint32_t GetElementCount() const;
    // uint32_t GetOffset() const;

private:
    VkBuffer m_handle{ VK_NULL_HANDLE };
    VmaAllocation m_allocation{ VK_NULL_HANDLE };
    VmaAllocationInfo m_info{};
    VkDeviceAddress m_deviceAddress{};

    VkBufferUsageFlags m_usage{};
    VkMemoryPropertyFlags m_memoryPropertyFlags{};

    bool m_mapped{};
    bool m_empty{ true };
};
}
