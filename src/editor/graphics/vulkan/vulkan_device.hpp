#pragma once

#include <vulkan/vulkan_core.h>

#include <optional>
#include <vector>

namespace Zeus
{
struct QueueFamilies
{
    std::optional<std::uint32_t> graphicsFamily;
    std::optional<std::uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SurfaceDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct VulkanDevice
{
    VkPhysicalDevice physicalDevice;
    VkDevice logicalDevice;

    SurfaceDetails surfaceDetails;
    QueueFamilies queueFamilies;

    VkQueue graphicsQueue;
    VkQueue presentQueue;
};

bool selectVkPhysicalDevice(
    const VkInstance& instance,
    const VkSurfaceKHR& surface,
    VkPhysicalDevice& physicalDevice);

bool createVkDevice(
    const VkPhysicalDevice& physicalDevice,
    const QueueFamilies& queueFamilies,
    VkDevice& device);

bool physicalDeviceMeetsRequirements(
    const VkPhysicalDevice& device,
    const VkSurfaceKHR& surface);

bool areDeviceExtensionSupported(
    const VkPhysicalDevice& device,
    const std::vector<const char*>& deviceExtensions);

const QueueFamilies getQueueFamilies(
    const VkPhysicalDevice& physicalDevice,
    const VkSurfaceKHR& surface);

const SurfaceDetails getSurfaceDetails(
    const VkPhysicalDevice& physicalDevice,
    const VkSurfaceKHR& surface);
}
