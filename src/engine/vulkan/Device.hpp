#pragma once

#include "Definitions.hpp"
#include "DeletionQueue.hpp"

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
        std::function<void(VkCommandBuffer cmd)>&& function);

    // CommandBuffer* CmdImmediateBegin(const QueueType queue_type);
    // void CmdImmediateSubmit(CommandBuffer* cmd);

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

    VkFence m_ImmediateSubmitFence{ VK_NULL_HANDLE };
    VkCommandPool m_ImmediateSubmitCommandPool{ VK_NULL_HANDLE };
    VkCommandBuffer m_ImmediateSubmitCommandBuffer{ VK_NULL_HANDLE };

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
