#pragma once

#include "vulkan/vulkan_memory.hpp"

#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace Zeus
{
class Buffer
{
public:
    Buffer() = default;

    Buffer(
        std::string_view name,
        VkBufferUsageFlags usage,
        VkDeviceSize itemSize,
        std::uint64_t count,
        VkMemoryPropertyFlags memoryPropertyFlags,
        bool mapped = false);

    Buffer(
        std::string_view name,
        VkBufferUsageFlags usage,
        VkDeviceSize size,
        VkMemoryPropertyFlags memoryPropertyFlags,
        bool mapped = false);

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    Buffer(Buffer&& other) noexcept;
    Buffer& operator=(Buffer&& other);

    ~Buffer();

    void Destroy();

    void Update(const void* data, std::size_t size, std::size_t srcOffset = 0);
    void UpdateAt(const void* data, std::size_t size, std::uint64_t index);

    void* GetData() const;

    template <typename T>
    inline T* GetData(std::uint64_t index = 0) const
    {
        assert(m_mapped);
        assert(index < m_count && "Invalid index");
        assert(sizeof(T) <= m_stride);

        auto offset{ reinterpret_cast<std::byte*>(m_info.pMappedData) +
                     (index * m_stride) };

        return reinterpret_cast<T*>(offset);
    }

    VkBuffer GetHandle() const;
    VmaAllocation GetAllocation() const;
    const VmaAllocationInfo& GetInfo() const;
    VkDeviceSize GetSize() const;
    VkDeviceAddress GetDeviceAddress() const;
    VkDeviceSize GetStride() const;
    std::uint64_t GetCount() const;
    VkBufferUsageFlags GetUsage() const;

    inline constexpr std::string_view GetName() const
    {
        return m_name;
    }

private:
    VkBuffer m_handle{ VK_NULL_HANDLE };
    VmaAllocation m_allocation{ VK_NULL_HANDLE };
    VmaAllocationInfo m_info{};
    VkDeviceAddress m_deviceAddress{};
    VkDeviceSize m_stride{};
    std::uint64_t m_count{};
    VkBufferUsageFlags m_usage{};
    std::string_view m_name;
    bool m_mapped{};
};
}
