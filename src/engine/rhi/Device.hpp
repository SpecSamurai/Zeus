#pragma once

#include "CommandBuffer.hpp"
#include "CommandPool.hpp"
#include "Definitions.hpp"
#include "Fence.hpp"
#include "Queue.hpp"

#include <vulkan/vulkan_core.h>

#include <functional>

namespace Zeus
{
struct DeviceMemoryBudget
{
    VkDeviceSize budgetBytes;
    VkDeviceSize usageBytes;
};

class Device
{
public:
    void Initialize(VkInstance instance, VkSurfaceKHR surface);
    void Destroy();

    void Wait() const;
    void WaitAll() const;

    void CmdImmediateSubmit(
        std::function<void(const CommandBuffer& cmd)>&& function);

    const CommandBuffer& BeginImmediateCmdBuffer();
    void SubmitImmediateCmdBuffer();

    const Queue& GetQueue(QueueType type) const;

    DeviceMemoryBudget GetMemoryBudget() const;

    VkDevice GetLogicalDevice() const;
    VkPhysicalDevice GetPhysicalDevice() const;

    const VkPhysicalDeviceLimits& GetLimits() const;

private:
    VkDevice m_logicalDevice{ VK_NULL_HANDLE };
    VkPhysicalDevice m_physicalDevice{ VK_NULL_HANDLE };

    Fence m_ImmediateSubmitFence;
    CommandPool m_ImmediateSubmitCommandPool;
    CommandBuffer m_ImmediateSubmitCommandBuffer;

    Queue m_graphicsQueue;
    Queue m_presentQueue;
    Queue m_transferQueue;
    Queue m_computeQueue;

    VkPhysicalDeviceLimits m_limits;
};
}
