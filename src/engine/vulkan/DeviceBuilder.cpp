#include "DeviceBuilder.hpp"

#include "Device.hpp"

#include "vulkan_settings.hpp"
#include "vulkan_utils.hpp"

#include <set>

namespace Zeus
{
Device DeviceBuilder::build()
{
    Device device{};

    device.physicalDevice = physicalDevice.handle;
    device.msaaSamples = physicalDevice.msaaSamples;

    device.graphicsFamily =
        physicalDevice.queueFamilies.graphicsFamily.value_or(0);
    device.presentFamily =
        physicalDevice.queueFamilies.presentFamily.value_or(0);
    device.transferFamily =
        physicalDevice.queueFamilies.transferFamily.value_or(0);
    device.computeFamily =
        physicalDevice.queueFamilies.computeFamily.value_or(0);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<std::uint32_t> uniqueQueueFamilies{
        device.graphicsFamily,
        device.presentFamily,
        device.transferFamily,
        device.computeFamily,
    };

    float queuePriority{ 1.0f };
    for (std::uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount =
        static_cast<std::uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = nullptr;
    createInfo.enabledExtensionCount =
        static_cast<std::uint32_t>(physicalDevice.extensions.size());
    createInfo.ppEnabledExtensionNames = physicalDevice.extensions.data();
    createInfo.pEnabledFeatures = &physicalDevice.features;

#ifndef NDEBUG
    // Deprecated but set for backwards compatibility
    createInfo.enabledLayerCount =
        static_cast<std::uint32_t>(INSTANCE_DEFAULT.VALIDATION_LAYERS.size());
    createInfo.ppEnabledLayerNames = INSTANCE_DEFAULT.VALIDATION_LAYERS.data();
#endif

    VkResult result{ vkCreateDevice(
        physicalDevice.handle,
        &createInfo,
        nullptr,
        &device.logicalDevice) };

    VKCHECK(result, "Failed to create logical device.");

    vkGetDeviceQueue(
        device.logicalDevice,
        device.graphicsFamily,
        0,
        &device.graphicsQueue);

    if (physicalDevice.queueFamilies.presentFamily.has_value())
    {
        vkGetDeviceQueue(
            device.logicalDevice,
            device.presentFamily,
            0,
            &device.presentQueue);
    }

    vkGetDeviceQueue(
        device.logicalDevice,
        device.transferFamily,
        0,
        &device.transferQueue);

    vkGetDeviceQueue(
        device.logicalDevice,
        device.computeFamily,
        0,
        &device.computeQueue);

    return device;
}

DeviceBuilder::DeviceBuilder(const PhysicalDevice& physicalDevice)
    : physicalDevice{ physicalDevice }
{
}
}
