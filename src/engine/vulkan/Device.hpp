#pragma once

#include "Definitions.hpp"
#include "DeletionQueue.hpp"
#include "DeviceBuilder.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>

namespace Zeus
{
class Device
{
public:
    void Init(const DeviceInfo& deviceInfo);
    void Destroy();

    void Wait();
    void WaitAll();

    // CommandBuffer* CmdImmediateBegin(const QueueType queue_type);
    // void CmdImmediateSubmit(CommandBuffer* cmd);

    VkQueue GetQueue(QueueType type) const;
    std::uint32_t GetQueueFamily(QueueType type) const;

    const VkDevice& GetLogicalDevice() const;
    const VkPhysicalDevice& GetPhysicalDevice() const;
    DeletionQueue& GetDeletionQueue();

private:
    VkDevice m_logicalDevice{ VK_NULL_HANDLE };
    VkPhysicalDevice m_physicalDevice{ VK_NULL_HANDLE };
    DeletionQueue m_deletionQueue;

    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;

    std::uint32_t graphicsFamily;
    std::uint32_t presentFamily;
    std::uint32_t transferFamily;
    std::uint32_t computeFamily;

    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkQueue transferQueue;
    VkQueue computeQueue;
};
}
