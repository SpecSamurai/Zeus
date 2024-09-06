#pragma once

#include "vulkan_memory.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>

namespace Zeus
{
struct Device
{
    VkDevice logicalDevice;
    VkPhysicalDevice physicalDevice;

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

inline void destroyDevice(Device& device)
{
    vkDestroyDevice(device.logicalDevice, allocationCallbacks.get());
}
}
