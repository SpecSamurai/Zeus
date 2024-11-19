#pragma once

#include "CommandBuffer.hpp"
#include "CommandPool.hpp"
#include "Definitions.hpp"
#include "DeletionQueue.hpp"
#include "Fence.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <functional>

namespace Zeus
{
class Device
{
public:
    void Init(VkInstance instance, VkSurfaceKHR surface);
    void Destroy();

    void Wait();
    void WaitAll();

    void CmdImmediateSubmit(
        std::function<void(const CommandBuffer& cmd)>&& function);

    VkQueue GetQueue(QueueType type) const;
    std::uint32_t GetQueueFamily(QueueType type) const;

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

    VkQueue m_graphicsQueue{ VK_NULL_HANDLE };
    VkQueue m_presentQueue{ VK_NULL_HANDLE };
    VkQueue m_transferQueue{ VK_NULL_HANDLE };
    VkQueue m_computeQueue{ VK_NULL_HANDLE };

    std::uint32_t m_graphicsFamily;
    std::uint32_t m_presentFamily;
    std::uint32_t m_transferFamily;
    std::uint32_t m_computeFamily;

    std::uint32_t m_maxImageDimension2D;
    std::uint32_t m_maxPushConstantsSize;
};
}
