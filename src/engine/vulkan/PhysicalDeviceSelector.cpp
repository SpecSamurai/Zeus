#include "PhysicalDeviceSelector.hpp"

#include "PhysicalDevice.hpp"

#include "core/logger.hpp"
#include "vulkan_device.hpp"
#include "vulkan_settings.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace Zeus
{
std::optional<PhysicalDevice> PhysicalDeviceSelector::select()
{
    if (!validate())
        return std::nullopt;

    std::uint32_t physicalDeviceCount{ 0 };
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

    if (physicalDeviceCount == 0)
    {
        error("Failed to find GPUs with Vulkan support");
        return std::nullopt;
    }

    std::vector<VkPhysicalDevice> devices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, devices.data());

    std::multimap<int, PhysicalDevice> candidates;

    for (const auto& device : devices)
    {
        auto physicalDevice = createIfValid(device);

        if (physicalDevice.has_value())
        {
            int score{ ratePhysicalDevice(physicalDevice.value()) };
            candidates.insert(std::make_pair(score, physicalDevice.value()));
        }
    }

    if (candidates.rbegin()->first > 0)
    {
        const auto& device = candidates.rbegin()->second;
        debug("Selected physical device: {}", device.name);
        debug(
            "GRAPHICS: {} | PRESENT: {} | COMPUTE: {} | TRANSFER: {}",
            device.queueFamilies.graphicsFamily.value(),
            device.queueFamilies.presentFamily.value(),
            device.queueFamilies.computeFamily.value(),
            device.queueFamilies.transferFamily.value());

        return device;
    }
    else
    {
        error("Failed to find a suitable physical device");
        return std::nullopt;
    }
}

bool PhysicalDeviceSelector::validate()
{
    bool isValid{ true };

    if (instance == VK_NULL_HANDLE)
    {
        error("Instance not set.");
        isValid = false;
    }

    if (criteria.requirePresent && surface == VK_NULL_HANDLE)
    {
        error("Present is required but Surface is not set.");
        isValid = false;
    }

    if (criteria.requirePresent && criteria.extensions.empty())
    {
        warning("Present is required but Extensions are not set. Use default.");
        criteria.extensions = INSTANCE_DEFAULT.DEVICE_EXTENSIONS;
    }

    return isValid;
}

std::optional<PhysicalDevice> PhysicalDeviceSelector::createIfValid(
    const VkPhysicalDevice& physicalDevice)
{
    PhysicalDevice output{};

    if (!areDeviceExtensionSupported(physicalDevice, criteria.extensions))
    {
        return std::nullopt;
    }

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    output.name = std::string(deviceProperties.deviceName);
    output.handle = physicalDevice;
    output.surface = surface;
    output.extensions = criteria.extensions;

    PhysicalDevice::QueueFamiliesInfo queueFamiliesInfo{};

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
            if (criteria.requirePresent)
            {
                VkBool32 surfaceSupport{ false };
                vkGetPhysicalDeviceSurfaceSupportKHR(
                    physicalDevice,
                    index,
                    surface,
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

    if (criteria.requirePresent && !queueFamiliesInfo.presentFamily.has_value())
        return std::nullopt;

    if (criteria.dedicatedTransferQueue)
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

    if (criteria.dedicatedComputeQueue)
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

    output.queueFamilies = queueFamiliesInfo;

    criteria.features1_2.pNext = &criteria.features1_3;
    criteria.features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    criteria.features2.pNext = &criteria.features1_2;
    if (!checkPhysicalDeviceFeatures(physicalDevice))
    {
        return std::nullopt;
    }

    output.features = criteria.features;
    output.features2 = criteria.features2;
    output.properties = deviceProperties;

    output.msaaSamples = getMaxUsableSampleCount(physicalDevice);

    return output;
}

int PhysicalDeviceSelector::ratePhysicalDevice(
    const PhysicalDevice& physicalDevice)
{
    int score{ 0 };

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice.handle, &deviceProperties);

    if (deviceProperties.deviceType == criteria.preferredType)
        score += 1000;

    return score;
}

bool PhysicalDeviceSelector::checkPhysicalDeviceFeatures(
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

    if (criteria.features2.features.samplerAnisotropy &&
        !deviceFeatures2.features.samplerAnisotropy)
    {
        error("Device doesn't support samplerAnisotropy.");
        isValid = false;
    }

    if (criteria.features2.features.fillModeNonSolid &&
        !deviceFeatures2.features.fillModeNonSolid)
    {
        error("Device doesn't support fillModeNonSolid.");
        isValid = false;
    }

    if (criteria.features2.features.sampleRateShading &&
        !deviceFeatures2.features.sampleRateShading)
    {
        error("Device doesn't support sampleRateShading.");
        isValid = false;
    }

    if (criteria.features1_2.bufferDeviceAddress &&
        !features1_2.bufferDeviceAddress)
    {
        error("Device doesn't support VkPhysicalDeviceVulkan12Features "
              "bufferDeviceAddress.");
        isValid = false;
    }

    if (criteria.features1_2.descriptorIndexing &&
        !features1_2.descriptorIndexing)
    {
        error("Device doesn't support VkPhysicalDeviceVulkan12Features "
              "descriptorIndexing.");
        isValid = false;
    }

    if (criteria.features1_3.dynamicRendering && !features1_3.dynamicRendering)
    {
        error("Device doesn't support VkPhysicalDeviceVulkan13Features "
              "dynamicRendering.");
        isValid = false;
    }

    if (criteria.features1_3.synchronization2 && !features1_3.synchronization2)
    {
        error("Device doesn't support VkPhysicalDeviceVulkan13Features "
              "synchronization2.");
        isValid = false;
    }

    return isValid;
}

PhysicalDeviceSelector::PhysicalDeviceSelector(const VkInstance& instance)
{
    this->instance = instance;
}

void PhysicalDeviceSelector::setSurface(const VkSurfaceKHR& surface)
{
    this->surface = surface;
}

void PhysicalDeviceSelector::setPreferredType(
    VkPhysicalDeviceType preferredType)
{
    criteria.preferredType = preferredType;
}

void PhysicalDeviceSelector::requirePresent(bool require)
{
    criteria.requirePresent = require;
}

void PhysicalDeviceSelector::dedicatedTransferQueue(bool dedicated)
{
    criteria.dedicatedTransferQueue = dedicated;
}

void PhysicalDeviceSelector::dedicatedComputeQueue(bool dedicated)
{
    criteria.dedicatedComputeQueue = dedicated;
}

void PhysicalDeviceSelector::addExtensions(
    const std::vector<const char*>& extensions)
{
    for (const auto& extension : extensions)
    {
        criteria.extensions.push_back(extension);
    }
}

void PhysicalDeviceSelector::setFeatures(
    const VkPhysicalDeviceFeatures& features)
{
    criteria.features2.features = features;
    criteria.features = features;
}

void PhysicalDeviceSelector::setFeatures1_2(
    const VkPhysicalDeviceVulkan12Features& features)
{
    criteria.features1_2 = features;
}

void PhysicalDeviceSelector::setFeatures1_3(
    const VkPhysicalDeviceVulkan13Features& features)
{
    criteria.features1_3 = features;
}
}
