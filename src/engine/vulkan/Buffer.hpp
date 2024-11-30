#pragma once

#include "rhi/vulkan_memory.hpp"

#include <vulkan/vulkan_core.h>

#include <cstddef>
#include <string_view>

namespace Zeus
{
class Buffer
{
public:
    Buffer() = default;
    Buffer(
        VkBufferUsageFlags usage,
        VkDeviceSize size,
        VkMemoryPropertyFlags memoryPropertyFlags,
        bool mapped = false,
        std::string_view name = "");

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    Buffer(Buffer&& other) noexcept;
    Buffer& operator=(Buffer&& other);

    ~Buffer();

    void Destroy();

    void Update(void* data, std::size_t size, std::size_t srcOffset = 0);

    void* GetData() const;
    VkBuffer GetHandle() const;
    VmaAllocation GetAllocation() const;
    const VmaAllocationInfo& GetInfo() const;
    VkDeviceSize GetSize() const;
    VkDeviceAddress GetDeviceAddress() const;

    VkBufferUsageFlags GetUsage() const;

private:
    VkBuffer m_handle{ VK_NULL_HANDLE };
    VmaAllocation m_allocation{ VK_NULL_HANDLE };
    VmaAllocationInfo m_info{};
    VkDeviceAddress m_deviceAddress{};

    VkBufferUsageFlags m_usage{};

    bool m_mapped;
};
}
