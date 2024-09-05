#pragma once

#include <vulkan/vulkan.h>

#include <optional>
#include <string>
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

    std::string name{};

    VkPhysicalDevice handle{ VK_NULL_HANDLE };
    VkSurfaceKHR surface{ VK_NULL_HANDLE };

    // Requested features
    VkPhysicalDeviceFeatures features{};
    VkPhysicalDeviceFeatures2 features2{};
    VkPhysicalDeviceProperties properties{};

    std::vector<const char*> extensions{};
};
}
