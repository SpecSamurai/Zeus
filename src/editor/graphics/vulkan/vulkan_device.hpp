#pragma once

#include <vulkan/vulkan_core.h>

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
    const VkPhysicalDevice& physicalDevice,
    const VkSurfaceKHR& surface);

VkSampleCountFlagBits getMaxUsableSampleCount(
    const VkPhysicalDevice& physicalDevice);

bool areDeviceExtensionSupported(
    const VkPhysicalDevice& device,
    const std::vector<const char*>& deviceExtensions);
}
