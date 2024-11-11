#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Zeus
{
struct SurfaceDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

const SurfaceDetails getSurfaceDetails(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface);

VkSampleCountFlagBits getMaxUsableSampleCount(VkPhysicalDevice physicalDevice);

bool areDeviceExtensionSupported(
    VkPhysicalDevice device,
    const std::vector<const char*>& deviceExtensions);
}
