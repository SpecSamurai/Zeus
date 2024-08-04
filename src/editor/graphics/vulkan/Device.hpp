#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>

namespace Zeus
{
struct Device
{
    VkDevice logicalDevice{ VK_NULL_HANDLE };
    VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };

    VkSampleCountFlagBits msaaSamples;

    // swapchainsupport swapchainsupport;
    // VkCommandPool graphics_command_pool;

    // VkPhysicalDeviceProperties properties;
    // VkPhysicalDeviceFeatures features;
    // VkPhysicalDeviceMemoryProperties memory;
    // VkFormat depthFormat;

    std::uint32_t graphicsFamily;
    std::uint32_t presentFamily;
    std::uint32_t transferFamily;
    std::uint32_t computeFamily;

    VkQueue graphicsQueue{ VK_NULL_HANDLE };
    VkQueue presentQueue{ VK_NULL_HANDLE };
    VkQueue transferQueue{ VK_NULL_HANDLE };
    VkQueue computeQueue{ VK_NULL_HANDLE };
};

inline void destroyDevice(Device& device)
{
    vkDestroyDevice(device.logicalDevice, nullptr);
}
}
