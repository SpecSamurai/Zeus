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
bool createVulkanDevice(
    const VkInstance& instance,
    const VkSurfaceKHR& surface,
    VulkanDevice& vulkanDevice)
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

    bool deviceFound{false};
    for (const auto& physicalDevice : physicalDevices)
    {
        if (!areDeviceExtensionSupported(physicalDevice, DEVICE_EXTENSIONS))
        {
            continue;
        }

        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

        auto queueFamilies = getQueueFamilies(physicalDevice, surface);
        auto surfaceDetails = getSurfaceDetails(physicalDevice, surface);

        bool isForSwapchain{
            !surfaceDetails.formats.empty() &&
            !surfaceDetails.presentModes.empty()};

        if (queueFamilies.isComplete() && isForSwapchain &&
            deviceFeatures.samplerAnisotropy)
        {
            vulkanDevice.physicalDevice = physicalDevice;
            vulkanDevice.queueFamilies = queueFamilies;
            vulkanDevice.surfaceDetails = surfaceDetails;
            vulkanDevice.msaaSamples = getMaxUsableSampleCount(physicalDevice);
            break;
        }
    }

    if (deviceFound)
    {
        error("Failed to find a suitable GPU");
        return false;
    }

    if (!createVkDevice(
            vulkanDevice.physicalDevice,
            vulkanDevice.queueFamilies,
            vulkanDevice.logicalDevice))
    {
        return false;
    }

    vkGetDeviceQueue(
        vulkanDevice.logicalDevice,
        vulkanDevice.queueFamilies.graphicsFamily.value(),
        0,
        &vulkanDevice.graphicsQueue);

    vkGetDeviceQueue(
        vulkanDevice.logicalDevice,
        vulkanDevice.queueFamilies.presentFamily.value(),
        0,
        &vulkanDevice.presentQueue);

    vkGetDeviceQueue(
        vulkanDevice.logicalDevice,
        vulkanDevice.queueFamilies.transferFamily.value(),
        0,
        &vulkanDevice.transferQueue);

    vkGetDeviceQueue(
        vulkanDevice.logicalDevice,
        vulkanDevice.queueFamilies.computeFamily.value(),
        0,
        &vulkanDevice.computeQueue);

    return true;
}

bool createVkDevice(
    const VkPhysicalDevice& physicalDevice,
    const QueueFamilies& queueFamilies,
    VkDevice& device)
{
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
        .sampleRateShading = VK_TRUE,
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

#ifndef NDEBUG
    // Deprecated but set to be backwards compatibility
    createInfo.enabledLayerCount =
        static_cast<std::uint32_t>(VALIDATION_LAYERS.size());
    createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
#endif

    VkResult result{
        vkCreateDevice(physicalDevice, &createInfo, nullptr, &device)};
    if (result != VK_SUCCESS)
    {
        error("Failed to create logical device. {}", vkResultToString(result));
    }

    return result == VK_SUCCESS;
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

    std::uint32_t surfaceFormatsCount{0};
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

    std::uint32_t presentModesCount{0};
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

    for (std::uint32_t index{0}; index < queueFamilies.size(); ++index)
    {
        if (queueFamilies[index].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            output.graphicsFamily = index;
        }

        if (queueFamilies[index].queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            output.transferFamily = index;
        }

        if (queueFamilies[index].queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            output.computeFamily = index;
        }

        VkBool32 surfaceSupport{false};
        vkGetPhysicalDeviceSurfaceSupportKHR(
            physicalDevice,
            index,
            surface,
            &surfaceSupport);

        if (surfaceSupport)
        {
            output.presentFamily = index;
        }

        if (output.isComplete())
        {
            break;
        }
    }

    return output;
}

VkSampleCountFlagBits getMaxUsableSampleCount(
    const VkPhysicalDevice& physicalDevice)
{
    VkPhysicalDeviceProperties physicalDeviceProperties{};
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

    VkSampleCountFlags sampleCountFlags{
        physicalDeviceProperties.limits.framebufferColorSampleCounts &
        physicalDeviceProperties.limits.framebufferDepthSampleCounts};

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
}
