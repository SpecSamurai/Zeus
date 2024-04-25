#include "logger.hpp"
#include "vulkan_settings.hpp"

#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cstdint>
#include <optional>
#include <set>
#include <vector>

namespace Zeus
{
struct QueueFamilyIndices
{
    std::optional<std::uint32_t> graphicsFamily;
    std::optional<std::uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VulkanDevice
{
public:
    VkPhysicalDevice physicalDevice{VK_NULL_HANDLE};

    void PickPhysicalDevice(
        const VkInstance& instance,
        const VkSurfaceKHR& surface)
    {
        std::uint32_t deviceCount{0};
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0)
        {
            throw std::runtime_error(
                "failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for (const auto& device : devices)
        {
            if (IsDeviceSuitable(device, surface))
            {
                physicalDevice = device;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE)
        {
            critical("Failed to find a suitable GPU");
            assert(false);
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    QueueFamilyIndices FindQueueFamilies(
        VkPhysicalDevice device,
        const VkSurfaceKHR& surface)
    {
        QueueFamilyIndices indices;

        std::uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(
            device,
            &queueFamilyCount,
            nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(
            device,
            &queueFamilyCount,
            queueFamilies.data());

        std::uint32_t i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(
                device,
                i,
                surface,
                &presentSupport);

            if (presentSupport)
            {
                indices.presentFamily = i;
            }
            if (indices.isComplete())
            {
                break;
            }

            i++;
        }

        return indices;
    }

    bool IsDeviceSuitable(VkPhysicalDevice device, const VkSurfaceKHR& surface)
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        // return deviceProperties.deviceType ==
        //            VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
        //        deviceFeatures.geometryShader;

        bool isSuitable = // deviceProperties.deviceType ==
                          //    VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
            deviceFeatures.geometryShader;

        if (isSuitable)
        {
            std::uint32_t major_version =
                VK_VERSION_MAJOR(deviceProperties.apiVersion);
            std::uint32_t minor_version =
                VK_VERSION_MINOR(deviceProperties.apiVersion);
            std::int32_t patch_version =
                VK_VERSION_PATCH(deviceProperties.apiVersion);
            debug("Name: {}", deviceProperties.deviceName);
            debug(
                "Api Version: {}.{}.{}",
                major_version,
                minor_version,
                patch_version);
            // debug("Driver Version: {}", deviceProperties.driverVersion);
        }

        QueueFamilyIndices indices = FindQueueFamilies(device, surface);

        bool extensionsSupported = CheckDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported)
        {
            SwapChainSupportDetails swapChainSupport =
                querySwapChainSupport(device, surface);
            swapChainAdequate = !swapChainSupport.formats.empty() &&
                                !swapChainSupport.presentModes.empty();
        }

        return isSuitable && indices.isComplete() && extensionsSupported &&
               swapChainAdequate;
    }

    bool CheckDeviceExtensionSupport(VkPhysicalDevice device)
    {
        std::uint32_t extensionCount;
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
            DEVICE_EXTENSIONS.begin(),
            DEVICE_EXTENSIONS.end());

        for (const auto& extension : availableExtensions)
        {
            if (requiredExtensions.contains(extension.extensionName))
            {
                debug(
                    "Device level extension supported: {}",
                    extension.extensionName);
            }

            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    SwapChainSupportDetails querySwapChainSupport(
        VkPhysicalDevice device,
        const VkSurfaceKHR& surface)
    {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            device,
            surface,
            &details.capabilities);

        std::uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            device,
            surface,
            &formatCount,
            nullptr);

        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(
                device,
                surface,
                &formatCount,
                details.formats.data());
        }

        std::uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            device,
            surface,
            &presentModeCount,
            nullptr);

        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                device,
                surface,
                &presentModeCount,
                details.presentModes.data());
        }

        return details;
    }
};
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
