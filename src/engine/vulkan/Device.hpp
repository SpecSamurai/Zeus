#pragma once

#include "CommandBuffer.hpp"
#include "CommandPool.hpp"
#include "Definitions.hpp"
#include "DeletionQueue.hpp"
#include "Fence.hpp"
#include "Queue.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <functional>

namespace Zeus
{
class Device
{
public:
    void Initialize(VkInstance instance, VkSurfaceKHR surface);
    void Destroy();

    void Wait();
    void WaitAll();

    void CmdImmediateSubmit(
        std::function<void(const CommandBuffer& cmd)>&& function);

    const Queue& GetQueue(QueueType type) const;

    VkDevice GetLogicalDevice() const;
    VkPhysicalDevice GetPhysicalDevice() const;
    DeletionQueue& GetDeletionQueue();

    std::uint32_t GetMaxImageDimension2D() const;
    std::uint32_t GetMaxPushConstantsSize() const;

private:
    VkDevice m_logicalDevice{ VK_NULL_HANDLE };
    VkPhysicalDevice m_physicalDevice{ VK_NULL_HANDLE };
    DeletionQueue m_deletionQueue;

    Fence m_ImmediateSubmitFence;
    CommandPool m_ImmediateSubmitCommandPool;
    CommandBuffer m_ImmediateSubmitCommandBuffer;

    Queue m_graphicsQueue;
    Queue m_presentQueue;
    Queue m_transferQueue;
    Queue m_computeQueue;

    std::uint32_t m_maxImageDimension2D;
    std::uint32_t m_maxPushConstantsSize;
};
}
