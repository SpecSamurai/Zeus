#include "DeletionQueue.hpp"

#include "Definitions.hpp"
#include "api/vulkan_memory.hpp"
#include "logging/logger.hpp"

#include <vulkan/vulkan_core.h>

#include <cassert>
#include <mutex>
#include <unordered_map>

namespace Zeus
{
DeletionQueue::DeletionQueue(VkDevice device) : m_device{ device }
{
    m_resources.reserve(static_cast<std::size_t>(ResourceType::Count));
}

void DeletionQueue::Add(const ResourceType type, void* handle)
{
    if (handle == nullptr)
    {
        LOG_WARNING("Added uninitialized handle to Deletion Queue");
        return;
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    m_resources[type].emplace_back(handle);
}

void DeletionQueue::Clear()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto& pair : m_resources)
    {
        auto& [type, handles] = pair;

        for (void* handle : handles)
        {
            switch (type)
            {
            case ResourceType::Fence:
                vkDestroyFence(
                    m_device,
                    reinterpret_cast<VkFence>(handle),
                    allocationCallbacks.get());
                break;
            case ResourceType::Semaphore:
                vkDestroySemaphore(
                    m_device,
                    reinterpret_cast<VkSemaphore>(handle),
                    allocationCallbacks.get());
                break;
            default:
                assert(false && "Unknown resource type");
                break;
            }
        }
    }

    m_resources.clear();
}
}
