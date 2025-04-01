#include "DeletionQueue.hpp"

#include "Definitions.hpp"
#include "logging/logger.hpp"
#include "vulkan/vulkan_memory.hpp"

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

void DeletionQueue::Initialize(VkDevice device, VmaAllocator allocator)
{
    m_device = device;
    m_allocator = allocator;
    m_resources.reserve(static_cast<std::size_t>(ResourceType::Count));
}

void DeletionQueue::Add(const ResourceType type, void* handle)
{
    assert(m_device != VK_NULL_HANDLE);
    if (handle == nullptr)
    {
        LOG_WARNING(
            "Added uninitialized {} to Deletion Queue",
            resourceTypeToString(type));
        return;
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    m_resources[type].emplace_back(handle);
}

void DeletionQueue::AddBuffer(VkBuffer handle, VmaAllocation allocation)
{
    if (handle == VK_NULL_HANDLE || allocation == VK_NULL_HANDLE)
    {
        LOG_WARNING("Added uninitialized Buffer to Deletion Queue");
        return;
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    m_buffers.emplace_back(AllocatedBuffer{
        .handle = handle,
        .allocation = allocation,
    });
}

void DeletionQueue::AddImage(
    VkImage handle,
    VkImageView view,
    VmaAllocation allocation)
{
    if (handle == VK_NULL_HANDLE || view == VK_NULL_HANDLE ||
        allocation == VK_NULL_HANDLE)
    {
        LOG_WARNING("Added uninitialized Image to Deletion Queue");
        return;
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    m_images.emplace_back(AllocatedImage{
        .handle = handle,
        .view = view,
        .allocation = allocation,
    });
}

void DeletionQueue::Clear()
{
    assert(m_device != VK_NULL_HANDLE && m_allocator != VK_NULL_HANDLE);
    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto& buffer : m_buffers)
    {
        vmaDestroyBuffer(m_allocator, buffer.handle, buffer.allocation);
    }

    m_buffers.clear();

    for (auto& image : m_images)
    {
        if (image.view != VK_NULL_HANDLE)
        {
            vkDestroyImageView(m_device, image.view, allocationCallbacks.get());
        }

        vmaDestroyImage(m_allocator, image.handle, image.allocation);
    }

    m_images.clear();

    for (auto& pair : m_resources)
    {
        auto& [type, handles] = pair;

        for (void* handle : handles)
        {
            switch (type)
            {
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
                LOG_ERROR(
                    "Unknown resource type {}",
                    resourceTypeToString(type));
                break;
            }
        }
    }

    m_resources.clear();
}
}
