#include "PhysicalDeviceSelector.hpp"

#include "Definitions.hpp"
#include "PhysicalDevice.hpp"
#include "api/vulkan_device.hpp"
#include "logging/logger.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <map>
#include <optional>
#include <vector>

namespace Zeus
{
std::optional<PhysicalDevice> PhysicalDeviceSelector::Select(
    const PhysicalDeviceSelectorInfo& info)
{
    if (!Validate(info))
        return std::nullopt;

    std::uint32_t physicalDeviceCount{ 0 };
    vkEnumeratePhysicalDevices(info.instance, &physicalDeviceCount, nullptr);

    if (physicalDeviceCount == 0)
    {
        LOG_ERROR("Failed to find GPUs with Vulkan support");
        return std::nullopt;
    }

    std::vector<VkPhysicalDevice> devices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(
        info.instance,
        &physicalDeviceCount,
        devices.data());

    std::multimap<int, PhysicalDevice> candidates;

    for (const auto& device : devices)
    {
        auto physicalDevice = CreateIfValid(info, device);

        if (physicalDevice.has_value())
        {
            int score{ RatePhysicalDevice(info, physicalDevice.value()) };
            candidates.insert(std::make_pair(score, physicalDevice.value()));
        }
    }

    if (candidates.rbegin()->first > 0)
    {
        const auto& device = candidates.rbegin()->second;
        LOG_DEBUG("Selected physical device: {}", device.GetName());
        LOG_DEBUG(
            "GRAPHICS: {} | PRESENT: {} | COMPUTE: {} | TRANSFER: {}",
            device.GetQueueFamily(QueueType::Graphics),
            device.GetQueueFamily(QueueType::Present),
            device.GetQueueFamily(QueueType::Compute),
            device.GetQueueFamily(QueueType::Transfer));

        return device;
    }
    else
    {
        LOG_ERROR("Failed to find a suitable physical device");
        return std::nullopt;
    }
}

bool PhysicalDeviceSelector::Validate(const PhysicalDeviceSelectorInfo& info)
{
    bool isValid{ true };

    if (info.instance == VK_NULL_HANDLE)
    {
        LOG_ERROR("Instance not set.");
        isValid = false;
    }

    if (info.requirePresent && info.surface == VK_NULL_HANDLE)
    {
        LOG_ERROR("Present is required but Surface is not set.");
        isValid = false;
    }

    if (info.extensions.empty())
    {
        LOG_WARNING("Extensions are not set.");
    }

    if (info.requirePresent &&
        (std::find(
             info.extensions.begin(),
             info.extensions.end(),
             VK_KHR_SWAPCHAIN_EXTENSION_NAME) == info.extensions.end()))
    {
        LOG_ERROR("Present is required but extension is not set. "
                  "VK_KHR_SWAPCHAIN_EXTENSION_NAME");
        isValid = false;
    }

    return isValid;
}

std::optional<PhysicalDevice> PhysicalDeviceSelector::CreateIfValid(
    const PhysicalDeviceSelectorInfo& info,
    const VkPhysicalDevice& physicalDevice)
{
    if (!areDeviceExtensionSupported(physicalDevice, info.extensions))
    {
        return std::nullopt;
    }

    SelectedQueueFamiliesInfo queueFamiliesInfo{};

    std::uint32_t queueFamilyCount{ 0 };
    vkGetPhysicalDeviceQueueFamilyProperties(
        physicalDevice,
        &queueFamilyCount,
        nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(
        physicalDevice,
        &queueFamilyCount,
        queueFamilies.data());

    constexpr VkQueueFlags graphicsFlags{ VK_QUEUE_GRAPHICS_BIT |
                                          VK_QUEUE_TRANSFER_BIT |
                                          VK_QUEUE_COMPUTE_BIT };

    for (std::uint32_t index{ 0 }; index < queueFamilies.size(); ++index)
    {
        if ((queueFamilies[index].queueFlags & graphicsFlags) == graphicsFlags)
        {
            if (info.requirePresent)
            {
                VkBool32 surfaceSupport{ false };
                vkGetPhysicalDeviceSurfaceSupportKHR(
                    physicalDevice,
                    index,
                    info.surface,
                    &surfaceSupport);

                if (!surfaceSupport)
                    continue;

                queueFamiliesInfo.presentFamily = index;
            }

            queueFamiliesInfo.graphicsFamily = index;
            queueFamiliesInfo.transferFamily = index;
            queueFamiliesInfo.computeFamily = index;

            break;
        }
    }

    if (!queueFamiliesInfo.graphicsFamily.has_value())
        return std::nullopt;

    if (info.requirePresent && !queueFamiliesInfo.presentFamily.has_value())
        return std::nullopt;

    if (info.dedicatedTransferQueue)
    {
        for (std::uint32_t index{ 0 }; index < queueFamilies.size(); ++index)
        {
            // clang-format off
            if ((queueFamilies[index].queueFlags & VK_QUEUE_TRANSFER_BIT) &&
                (queueFamilies[index].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0 &&
                index != queueFamiliesInfo.graphicsFamily)
            // clang-format on
            {
                queueFamiliesInfo.transferFamily = index;
                break;
            }
        }
    }

    if (!queueFamiliesInfo.transferFamily.has_value())
        return std::nullopt;

    if (info.dedicatedComputeQueue)
    {
        for (std::uint32_t index{ 0 }; index < queueFamilies.size(); ++index)
        {
            // clang-format off
            if ((queueFamilies[index].queueFlags & VK_QUEUE_COMPUTE_BIT) &&
                index != queueFamiliesInfo.graphicsFamily)
            // clang-format on
            {
                queueFamiliesInfo.computeFamily = index;
                break;
            }
        }
    }

    if (!queueFamiliesInfo.computeFamily.has_value())
        return std::nullopt;

    if (!CheckPhysicalDeviceFeatures(info, physicalDevice))
    {
        return std::nullopt;
    }

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    QueueFamilies queues{
        .graphicsFamily = queueFamiliesInfo.graphicsFamily.value(),
        .presentFamily = queueFamiliesInfo.presentFamily.value(),
        .transferFamily = queueFamiliesInfo.transferFamily.value(),
        .computeFamily = queueFamiliesInfo.computeFamily.value(),
    };

    PhysicalDevice output(
        physicalDevice,
        deviceProperties.deviceType,
        queues,
        deviceProperties.deviceName);

    return output;
}

std::int32_t PhysicalDeviceSelector::RatePhysicalDevice(
    const PhysicalDeviceSelectorInfo& info,
    const PhysicalDevice& physicalDevice)
{
    int score{ 0 };

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(
        physicalDevice.GetHandle(),
        &deviceProperties);

    if (deviceProperties.deviceType == info.preferredType)
        score += 1000;

    return score;
}

bool PhysicalDeviceSelector::CheckPhysicalDeviceFeatures(
    const PhysicalDeviceSelectorInfo& info,
    VkPhysicalDevice physicalDevice)
{
    bool isValid{ true };

    VkPhysicalDeviceVulkan13Features features1_3{};
    features1_3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;

    VkPhysicalDeviceVulkan12Features features1_2{};
    features1_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features1_2.pNext = &features1_3;

    VkPhysicalDeviceFeatures2 deviceFeatures2{};
    deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    deviceFeatures2.pNext = &features1_2;

    vkGetPhysicalDeviceFeatures2(physicalDevice, &deviceFeatures2);

    if (info.features2.features.samplerAnisotropy &&
        !deviceFeatures2.features.samplerAnisotropy)
    {
        LOG_ERROR("Device doesn't support samplerAnisotropy.");
        isValid = false;
    }

    if (info.features2.features.fillModeNonSolid &&
        !deviceFeatures2.features.fillModeNonSolid)
    {
        LOG_ERROR("Device doesn't support fillModeNonSolid.");
        isValid = false;
    }

    if (info.features2.features.sampleRateShading &&
        !deviceFeatures2.features.sampleRateShading)
    {
        LOG_ERROR("Device doesn't support sampleRateShading.");
        isValid = false;
    }

    if (info.features1_2.bufferDeviceAddress &&
        !features1_2.bufferDeviceAddress)
    {
        LOG_ERROR("Device doesn't support VkPhysicalDeviceVulkan12Features "
                  "bufferDeviceAddress.");
        isValid = false;
    }

    if (info.features1_2.descriptorIndexing && !features1_2.descriptorIndexing)
    {
        LOG_ERROR("Device doesn't support VkPhysicalDeviceVulkan12Features "
                  "descriptorIndexing.");
        isValid = false;
    }

    if (info.features1_3.dynamicRendering && !features1_3.dynamicRendering)
    {
        LOG_ERROR("Device doesn't support VkPhysicalDeviceVulkan13Features "
                  "dynamicRendering.");
        isValid = false;
    }

    if (info.features1_3.synchronization2 && !features1_3.synchronization2)
    {
        LOG_ERROR("Device doesn't support VkPhysicalDeviceVulkan13Features "
                  "synchronization2.");
        isValid = false;
    }

    return isValid;
}
}
