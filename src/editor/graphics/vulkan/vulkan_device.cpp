#include "vulkan_device.hpp"

#include "vulkan_settings.hpp"
#include "vulkan_utils.hpp"

#include <core/logger.hpp>

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <set>
#include <vector>

namespace Zeus
{
bool selectVkPhysicalDevice(
    const VkInstance& instance,
    const VkSurfaceKHR& surface,
    VkPhysicalDevice& physicalDevice)
{
    std::uint32_t physicalDeviceCount{0};
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

    if (physicalDeviceCount == 0)
    {
        error("Failed to find GPUs with Vulkan support");
        return false;
    }

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(
        instance,
        &physicalDeviceCount,
        physicalDevices.data());

    VkPhysicalDevice output{VK_NULL_HANDLE};
    for (const auto& device : physicalDevices)
    {
        if (physicalDeviceMeetsRequirements(device, surface))
        {
            output = device;
            break;
        }
    }

    if (output == VK_NULL_HANDLE)
    {
        error("Failed to find a suitable GPU");
        return false;
    }

    physicalDevice = output;
    return true;
}

bool createVkDevice(
    const VkPhysicalDevice& physicalDevice,
    const QueueFamilies& queueFamilies,
    VkDevice& device)
{
    // QueueFamilyIndices indices =
    //     physical.FindQueueFamilies(physicalDevice, surface);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<std::uint32_t> uniqueQueueFamilies = {
        queueFamilies.graphicsFamily.value(),
        queueFamilies.presentFamily.value(),
    };

    float queuePriority{1.0f};
    for (std::uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = queueFamily,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority,
        };

        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures physicalDeviceFeatures{
        .samplerAnisotropy = VK_TRUE,
    };

    VkDeviceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount =
            static_cast<std::uint32_t>(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount =
            static_cast<std::uint32_t>(DEVICE_EXTENSIONS.size()),
        .ppEnabledExtensionNames = DEVICE_EXTENSIONS.data(),
        .pEnabledFeatures = &physicalDeviceFeatures,
    };

// Previous implementations of Vulkan made a distinction between
// instance and device specific validation layers, but this is no longer
// the case. That means that the enabledLayerCount and
// ppEnabledLayerNames fields of VkDeviceCreateInfo are ignored by
// up-to-date implementations. However, it is still a good idea to set
// them anyway to be compatible with older implementations:
// Depricated but set to be backwards compatible
#ifndef NDEBUG
    createInfo.enabledLayerCount =
        static_cast<std::uint32_t>(VALIDATION_LAYERS.size());
    createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
#endif

    VkResult result{
        vkCreateDevice(physicalDevice, &createInfo, nullptr, &device)};
    if (result != VK_SUCCESS)
    {
        error("Failed to create logical device. {}", vkResultToString(result));
        return false;
    }

    // vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0,
    // &graphicsQueue); vkGetDeviceQueue(device, indices.presentFamily.value(),
    // 0, &presentQueue);
    return true;
}

bool physicalDeviceMeetsRequirements(
    const VkPhysicalDevice& physicalDevice,
    const VkSurfaceKHR& surface)
{
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

    // return deviceProperties.deviceType ==
    //            VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
    //        deviceFeatures.geometryShader;
    // bool isSuitable = // deviceProperties.deviceType ==
    //                   //    VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
    //     deviceFeatures.geometryShader;
    // if (isSuitable)
    // {
    //     std::uint32_t major_version =
    //         VK_VERSION_MAJOR(deviceProperties.apiVersion);
    //     std::uint32_t minor_version =
    //         VK_VERSION_MINOR(deviceProperties.apiVersion);
    //     std::int32_t patch_version =
    //         VK_VERSION_PATCH(deviceProperties.apiVersion);
    //     debug("Name: {}", deviceProperties.deviceName);
    //     debug(
    //         "Api Version: {}.{}.{}",
    //         major_version,
    //         minor_version,
    //         patch_version);
    //     // debug("Driver Version: {}", deviceProperties.driverVersion);
    // }

    bool extensionsSupported{
        areDeviceExtensionSupported(physicalDevice, DEVICE_EXTENSIONS)};
    if (!extensionsSupported)
    {
        return false;
    }

    QueueFamilies queueFamilies = getQueueFamilies(physicalDevice, surface);
    SurfaceDetails surfaceDetails = getSurfaceDetails(physicalDevice, surface);

    bool isForSwapchain{
        !surfaceDetails.formats.empty() &&
        !surfaceDetails.presentModes.empty()};

    VkPhysicalDeviceFeatures supportedFeatures{};
    vkGetPhysicalDeviceFeatures(physicalDevice, &supportedFeatures);

    return queueFamilies.isComplete() && extensionsSupported &&
           isForSwapchain && supportedFeatures.samplerAnisotropy;
}

bool areDeviceExtensionSupported(
    const VkPhysicalDevice& device,
    const std::vector<const char*>& deviceExtensions)
{
    std::uint32_t extensionCount{0};
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

const SurfaceDetails getSurfaceDetails(
    const VkPhysicalDevice& physicalDevice,
    const VkSurfaceKHR& surface)
{
    SurfaceDetails details{};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physicalDevice,
        surface,
        &details.capabilities);

    std::uint32_t surfaceFormatCount{0};
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        physicalDevice,
        surface,
        &surfaceFormatCount,
        nullptr);

    if (surfaceFormatCount != 0)
    {
        details.formats.resize(surfaceFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            physicalDevice,
            surface,
            &surfaceFormatCount,
            details.formats.data());
    }

    std::uint32_t presentModeCount{0};
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        physicalDevice,
        surface,
        &presentModeCount,
        nullptr);

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            physicalDevice,
            surface,
            &presentModeCount,
            details.presentModes.data());
    }

    return details;
}

const QueueFamilies getQueueFamilies(
    const VkPhysicalDevice& physicalDevice,
    const VkSurfaceKHR& surface)
{
    QueueFamilies output{};

    std::uint32_t queueFamilyCount{0};
    vkGetPhysicalDeviceQueueFamilyProperties(
        physicalDevice,
        &queueFamilyCount,
        nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(
        physicalDevice,
        &queueFamilyCount,
        queueFamilies.data());

    std::uint32_t index{0};
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            output.graphicsFamily = index;
        }

        VkBool32 presentSupport{false};
        vkGetPhysicalDeviceSurfaceSupportKHR(
            physicalDevice,
            index,
            surface,
            &presentSupport);

        if (presentSupport)
        {
            output.presentFamily = index;
        }

        if (output.isComplete())
        {
            break;
        }

        ++index;
    }

    return output;
}
}

// void pickPhysicalDevice() {
//     ...
//
//     // Use an ordered map to automatically sort candidates by increasing
//     score std::multimap<int, VkPhysicalDevice> candidates;
//
//     for (const auto& device : devices) {
//         int score = rateDeviceSuitability(device);
//         candidates.insert(std::make_pair(score, device));
//     }
//
//     // Check if the best candidate is suitable at all
//     if (candidates.rbegin()->first > 0) {
//         physicalDevice = candidates.rbegin()->second;
//     } else {
//         throw std::runtime_error("failed to find a suitable GPU!");
//     }
// }
//
// int rateDeviceSuitability(VkPhysicalDevice device) {
//     ...
//
//     int score = 0;
//
//     // Discrete GPUs have a significant performance advantage
//     if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
//     {
//         score += 1000;
//     }
//
//     // Maximum possible size of textures affects graphics quality
//     score += deviceProperties.limits.maxImageDimension2D;
//
//     // Application can't function without geometry shaders
//     if (!deviceFeatures.geometryShader) {
//         return 0;
//     }
//
//     return score;
// }
