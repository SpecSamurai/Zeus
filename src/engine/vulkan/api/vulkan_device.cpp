#include "vulkan_device.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <set>
#include <string>
#include <vector>

namespace Zeus
{
const SurfaceDetails getSurfaceDetails(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface)
{
    SurfaceDetails details{};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physicalDevice,
        surface,
        &details.capabilities);

    std::uint32_t surfaceFormatsCount{ 0 };
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        physicalDevice,
        surface,
        &surfaceFormatsCount,
        nullptr);

    if (surfaceFormatsCount != 0)
    {
        details.formats.resize(surfaceFormatsCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            physicalDevice,
            surface,
            &surfaceFormatsCount,
            details.formats.data());
    }

    std::uint32_t presentModesCount{ 0 };
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        physicalDevice,
        surface,
        &presentModesCount,
        nullptr);

    if (presentModesCount != 0)
    {
        details.presentModes.resize(presentModesCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            physicalDevice,
            surface,
            &presentModesCount,
            details.presentModes.data());
    }

    return details;
}

VkSampleCountFlagBits getMaxUsableSampleCount(VkPhysicalDevice physicalDevice)
{
    VkPhysicalDeviceProperties physicalDeviceProperties{};
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

    VkSampleCountFlags sampleCountFlags{
        physicalDeviceProperties.limits.framebufferColorSampleCounts &
        physicalDeviceProperties.limits.framebufferDepthSampleCounts
    };

    if (sampleCountFlags & VK_SAMPLE_COUNT_64_BIT)
    {
        return VK_SAMPLE_COUNT_64_BIT;
    }
    if (sampleCountFlags & VK_SAMPLE_COUNT_32_BIT)
    {
        return VK_SAMPLE_COUNT_32_BIT;
    }
    if (sampleCountFlags & VK_SAMPLE_COUNT_16_BIT)
    {
        return VK_SAMPLE_COUNT_16_BIT;
    }
    if (sampleCountFlags & VK_SAMPLE_COUNT_8_BIT)
    {
        return VK_SAMPLE_COUNT_8_BIT;
    }
    if (sampleCountFlags & VK_SAMPLE_COUNT_4_BIT)
    {
        return VK_SAMPLE_COUNT_4_BIT;
    }
    if (sampleCountFlags & VK_SAMPLE_COUNT_2_BIT)
    {
        return VK_SAMPLE_COUNT_2_BIT;
    }

    return VK_SAMPLE_COUNT_1_BIT;
}

bool areDeviceExtensionSupported(
    VkPhysicalDevice device,
    const std::vector<const char*>& deviceExtensions)
{
    std::uint32_t extensionCount{ 0 };
    vkEnumerateDeviceExtensionProperties(
        device,
        nullptr,
        &extensionCount,
        nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(
        device,
        nullptr,
        &extensionCount,
        availableExtensions.data());

    std::set<std::string> requiredExtensions(
        deviceExtensions.begin(),
        deviceExtensions.end());

    for (const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}
}
