#include "DeviceBuilder.hpp"

#include "PhysicalDeviceSelector.hpp"
#include "api/vulkan_debug.hpp"
#include "api/vulkan_memory.hpp"

#include <vulkan/vulkan_core.h>

#include <set>

namespace Zeus
{
DeviceInfo DeviceBuilder::Build()
{
    PhysicalDeviceSelectorInfo info{
        .extensions = m_info.extensions,
        .instance = m_info.instance,
        .surface = m_info.surface,

        .preferredType = m_info.preferredType,

        .features2 = m_info.features2,
        .features = m_info.features,
        .features1_2 = m_info.features1_2,
        .features1_3 = m_info.features1_3,

        .requirePresent = m_info.requirePresent,
        .dedicatedTransferQueue = m_info.dedicatedTransferQueue,
        .dedicatedComputeQueue = m_info.dedicatedComputeQueue,
    };

    auto selectedDevice{ PhysicalDeviceSelector::Select(info) };
    if (!selectedDevice.has_value())
    {
        assert(false && "Physical Device not found.");
    }

    PhysicalDevice physicalDevice{ selectedDevice.value() };
    DeviceInfo device{};
    device.physicalDevice = physicalDevice.handle;
    device.graphicsFamily = physicalDevice.queueFamilies.graphicsFamily.value();
    device.presentFamily = physicalDevice.queueFamilies.presentFamily.value();
    device.transferFamily = physicalDevice.queueFamilies.transferFamily.value();
    device.computeFamily = physicalDevice.queueFamilies.computeFamily.value();

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
        static_cast<std::uint32_t>(m_info.extensions.size());
    createInfo.ppEnabledExtensionNames = m_info.extensions.data();
    // createInfo.pEnabledFeatures = &physicalDevice.features;
    createInfo.pNext = &m_info.features2;

#ifndef NDEBUG
    // Deprecated but set for backwards compatibility
    createInfo.enabledLayerCount =
        static_cast<std::uint32_t>(VALIDATION_LAYERS.size());
    createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
#endif

    VkResult result{ vkCreateDevice(
        physicalDevice.handle,
        &createInfo,
        allocationCallbacks.get(),
        &device.logicalDevice) };

    VKCHECK(result, "Failed to create logical device.");

    vkGetDeviceQueue(
        device.logicalDevice,
        device.graphicsFamily,
        0,
        &device.graphicsQueue);

    if (m_info.requirePresent)
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

void DeviceBuilder::SetInstance(const VkInstance& instance)
{
    m_info.instance = instance;
}

void DeviceBuilder::SetSurface(const VkSurfaceKHR& surface)
{
    m_info.surface = surface;
}

void DeviceBuilder::SetPreferredType(VkPhysicalDeviceType preferredType)
{
    m_info.preferredType = preferredType;
}

void DeviceBuilder::RequirePresent(bool require)
{
    m_info.requirePresent = require;
}

void DeviceBuilder::DedicatedTransferQueue(bool dedicated)
{
    m_info.dedicatedTransferQueue = dedicated;
}

void DeviceBuilder::DedicatedComputeQueue(bool dedicated)
{
    m_info.dedicatedComputeQueue = dedicated;
}

void DeviceBuilder::AddExtensions(const std::vector<const char*>& extensions)
{
    for (const auto& extension : extensions)
    {
        m_info.extensions.emplace_back(extension);
    }
}

void DeviceBuilder::SetFeatures(const VkPhysicalDeviceFeatures& features)
{
    m_info.features2.features = features;
    m_info.features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    m_info.features2.pNext = &m_info.features1_2;

    m_info.features = features;
}

void DeviceBuilder::SetFeatures1_2(
    const VkPhysicalDeviceVulkan12Features& features)
{
    m_info.features1_2 = features;
    m_info.features1_2.pNext = &m_info.features1_3;
}

void DeviceBuilder::SetFeatures1_3(
    const VkPhysicalDeviceVulkan13Features& features)
{
    m_info.features1_3 = features;
}
}
