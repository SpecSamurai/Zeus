#pragma once

#include <vulkan/vulkan_core.h>

#include <optional>
#include <vector>

namespace Zeus
{
struct PhysicalDevice
{
    struct QueueFamiliesInfo
    {
        std::optional<std::uint32_t> graphicsFamily;
        std::optional<std::uint32_t> presentFamily;
        std::optional<std::uint32_t> transferFamily;
        std::optional<std::uint32_t> computeFamily;
    } queueFamilies;

    const char* name{ nullptr };

    VkPhysicalDevice handle{ VK_NULL_HANDLE };
    VkSurfaceKHR surface{ VK_NULL_HANDLE };

    // Requested features
    // VkPhysicalDeviceFeatures features{};
    // VkPhysicalDeviceProperties properties{};
    // VkPhysicalDeviceMemoryProperties memoryProperties{};

    VkSampleCountFlagBits msaaSamples;
    // vulkanswapchain swapchain_support;
    // VkFormat depthFormat;

    std::vector<const char*> extensions{};
};
}
