#include "PhysicalDevice.hpp"

#include "Definitions.hpp"
#include "Handle.hpp"

#include <vulkan/vulkan_core.h>

#include <cassert>

namespace Zeus
{
PhysicalDevice::PhysicalDevice(
    VkPhysicalDevice handle,
    VkPhysicalDeviceType deviceType,
    const QueueFamilies& queueFamilies,
    const char* name)
    : Handle(name),
      m_handle{ handle },
      m_deviceType{ deviceType },
      m_queueFamilies{ queueFamilies }
{
}

std::uint32_t PhysicalDevice::GetQueueFamily(const QueueType type) const
{
    switch (type)
    {
    case QueueType::Graphics:
        return m_queueFamilies.graphicsFamily;
    case QueueType::Present:
        return m_queueFamilies.presentFamily;
    case QueueType::Transfer:
        return m_queueFamilies.transferFamily;
    case QueueType::Compute:
        return m_queueFamilies.computeFamily;
    default:
        assert(false && "Queue type not supported");
    }
}

VkPhysicalDevice PhysicalDevice::GetHandle() const
{
    return m_handle;
}

VkPhysicalDeviceType PhysicalDevice::GetDeviceType() const
{
    return m_deviceType;
}
}
