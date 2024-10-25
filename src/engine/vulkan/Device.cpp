#include "Device.hpp"

#include "api/vulkan_memory.hpp"

#include <vulkan/vulkan_core.h>

#include <utility>

namespace Zeus
{
void Device::Init(const DeviceInfo& deviceInfo)
{
    m_logicalDevice = std::move(deviceInfo.logicalDevice);
    m_physicalDevice = std::move(deviceInfo.physicalDevice);
    m_deletionQueue.Init(m_logicalDevice);

    graphicsFamily = deviceInfo.graphicsFamily;
    presentFamily = deviceInfo.presentFamily;
    computeFamily = deviceInfo.computeFamily;
    transferFamily = deviceInfo.transferFamily;

    graphicsQueue = deviceInfo.graphicsQueue;
    presentQueue = deviceInfo.presentQueue;
    computeQueue = deviceInfo.computeQueue;
    transferQueue = deviceInfo.transferQueue;
}

void Device::Destroy()
{
    vkDestroyDevice(m_logicalDevice, allocationCallbacks.get());
}

void Device::Wait()
{
    vkDeviceWaitIdle(m_logicalDevice);
}

void Device::WaitAll()
{
    // for (uint32_t i = 0; i < 2; i++)
    // {
    //     queues::regular[i]->Wait();
    // }
}

const VkDevice& Device::GetLogicalDevice() const
{
    return m_logicalDevice;
}

const VkPhysicalDevice& Device::GetPhysicalDevice() const
{
    return m_physicalDevice;
}

DeletionQueue& Device::GetDeletionQueue()
{
    return m_deletionQueue;
}

VkQueue Device::GetQueue(QueueType type) const
{
    switch (type)
    {
    case QueueType::Graphics:
        return graphicsQueue;
    case QueueType::Present:
        return presentQueue;
    case QueueType::Transfer:
        return transferQueue;
    case QueueType::Compute:
        return computeQueue;
    default:
        assert(false && "Queue type not supported");
    }
}

std::uint32_t Device::GetQueueFamily(QueueType type) const
{
    switch (type)
    {
    case QueueType::Graphics:
        return graphicsFamily;
    case QueueType::Present:
        return presentFamily;
    case QueueType::Transfer:
        return transferFamily;
    case QueueType::Compute:
        return computeFamily;
    default:
        assert(false && "Queue type not supported");
    }
}
}
