#pragma once

#include "Definitions.hpp"
#include "rhi/vulkan/vulkan_memory.hpp"

#include <vulkan/vulkan_core.h>

#include <cassert>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace Zeus
{
struct AllocatedBuffer
{
    VkBuffer handle;
    VmaAllocation allocation;
};

struct AllocatedImage
{
    VkImage handle;
    VkImageView view;
    VmaAllocation allocation;
};

class DeletionQueue
{
public:
    ~DeletionQueue();

    void Initialize(VkDevice device, VmaAllocator m_allocator);

    void Add(const ResourceType type, void* handle);
    void AddBuffer(VkBuffer handle, VmaAllocation allocation);
    void AddImage(VkImage handle, VkImageView view, VmaAllocation allocation);

    void Clear();

private:
    VkDevice m_device{ VK_NULL_HANDLE };
    VmaAllocator m_allocator{ VK_NULL_HANDLE };

    std::unordered_map<ResourceType, std::vector<void*>> m_resources;
    std::vector<AllocatedBuffer> m_buffers;
    std::vector<AllocatedImage> m_images;

    std::mutex m_mutex;
};
}
