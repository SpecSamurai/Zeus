#pragma once

#include "Definitions.hpp"
#include "Handle.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>

namespace Zeus
{
struct QueueFamilies
{
    std::uint32_t graphicsFamily;
    std::uint32_t presentFamily;
    std::uint32_t transferFamily;
    std::uint32_t computeFamily;
};

class PhysicalDevice : public Handle
{
public:
    PhysicalDevice(
        VkPhysicalDevice handle,
        VkPhysicalDeviceType deviceType,
        const QueueFamilies& queueFamilies,
        const char* name = nullptr);

    std::uint32_t GetQueueFamily(const QueueType type) const;
    VkPhysicalDevice GetHandle() const;
    VkPhysicalDeviceType GetDeviceType() const;

private:
    VkPhysicalDevice m_handle{ VK_NULL_HANDLE };
    VkPhysicalDeviceType m_deviceType;
    QueueFamilies m_queueFamilies;
};
}
