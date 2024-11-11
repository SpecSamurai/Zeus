#include "DeletionQueue.hpp"

#include "Buffer.hpp"
#include "Definitions.hpp"
#include "api/vulkan_memory.hpp"
#include "logging/logger.hpp"

#include <vulkan/vulkan_core.h>

#include <cassert>
#include <mutex>
#include <unordered_map>

namespace Zeus
{
DeletionQueue::~DeletionQueue()
{
    Clear();
}

void DeletionQueue::Init(VkDevice device)
{
    m_device = device;
    m_resources.reserve(static_cast<std::size_t>(ResourceType::Count));
}

void DeletionQueue::Add(const ResourceType type, void* handle)
{
    assert(m_device != VK_NULL_HANDLE);
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
    assert(m_device != VK_NULL_HANDLE);
    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto& pair : m_resources)
    {
        auto& [type, handles] = pair;

        for (void* handle : handles)
        {
            switch (type)
            {
            case ResourceType::Buffer:
                reinterpret_cast<Buffer*>(handle)->Destroy();
                break;
            case ResourceType::CommandPool:
                vkDestroyCommandPool(
                    m_device,
                    reinterpret_cast<VkCommandPool>(handle),
                    allocationCallbacks.get());
                break;
            case ResourceType::DescriptorPool:
                vkDestroyDescriptorPool(
                    m_device,
                    reinterpret_cast<VkDescriptorPool>(handle),
                    allocationCallbacks.get());
                break;
            case ResourceType::DescriptorSetLayout:
                vkDestroyDescriptorSetLayout(
                    m_device,
                    reinterpret_cast<VkDescriptorSetLayout>(handle),
                    allocationCallbacks.get());
                break;
            case ResourceType::Fence:
                vkDestroyFence(
                    m_device,
                    reinterpret_cast<VkFence>(handle),
                    allocationCallbacks.get());
                break;
            case ResourceType::Image:
                // vmaDestroyImage(allocator, image.image, image.allocation);
                vkDestroyImage(
                    m_device,
                    reinterpret_cast<VkImage>(handle),
                    allocationCallbacks.get());
                break;
            case ResourceType::ImageView:
                vkDestroyImageView(
                    m_device,
                    reinterpret_cast<VkImageView>(handle),
                    allocationCallbacks.get());
                break;
            case ResourceType::Pipeline:
                vkDestroyPipeline(
                    m_device,
                    reinterpret_cast<VkPipeline>(handle),
                    allocationCallbacks.get());
                break;
            case ResourceType::PipelineLayout:
                vkDestroyPipelineLayout(
                    m_device,
                    reinterpret_cast<VkPipelineLayout>(handle),
                    allocationCallbacks.get());
                break;
            case ResourceType::Semaphore:
                vkDestroySemaphore(
                    m_device,
                    reinterpret_cast<VkSemaphore>(handle),
                    allocationCallbacks.get());
                break;
            case ResourceType::Shader:
                vkDestroyShaderModule(
                    m_device,
                    reinterpret_cast<VkShaderModule>(handle),
                    allocationCallbacks.get());
                break;
            case ResourceType::Sampler:
                vkDestroySampler(
                    m_device,
                    reinterpret_cast<VkSampler>(handle),
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
