#include "PhysicalDeviceSelector.hpp"

#include "PhysicalDevice.hpp"
#include "vulkan_device.hpp"
#include "vulkan_settings.hpp"

#include <core/logger.hpp>

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <map>
#include <optional>
#include <vector>

namespace Zeus
{
std::optional<PhysicalDevice> PhysicalDeviceSelector::select()
{
    if (!validate())
        return std::nullopt;

    std::uint32_t physicalDeviceCount{ 0 };
    vkEnumeratePhysicalDevices(instance.handle, &physicalDeviceCount, nullptr);

    if (physicalDeviceCount == 0)
    {
        error("Failed to find GPUs with Vulkan support");
        return std::nullopt;
    }

    std::vector<VkPhysicalDevice> devices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(
        instance.handle,
        &physicalDeviceCount,
        devices.data());

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

    if (instance.handle == VK_NULL_HANDLE)
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

    output.name = deviceProperties.deviceName;
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

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

    if (!deviceFeatures.geometryShader || !deviceFeatures.samplerAnisotropy)
    {
        return std::nullopt;
    }

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

PhysicalDeviceSelector::PhysicalDeviceSelector(const Instance& instance)
{
    this->instance = instance;
}

PhysicalDeviceSelector& PhysicalDeviceSelector::setSurface(
    const VkSurfaceKHR& surface)
{
    this->surface = surface;
    return *this;
}

PhysicalDeviceSelector& PhysicalDeviceSelector::setPreferredType(
    VkPhysicalDeviceType preferredType)
{
    criteria.preferredType = preferredType;
    return *this;
}

PhysicalDeviceSelector& PhysicalDeviceSelector::requirePresent(bool require)
{
    criteria.requirePresent = require;
    return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::dedicatedTransferQueue(
    bool dedicated)
{
    criteria.dedicatedTransferQueue = dedicated;
    return *this;
}
PhysicalDeviceSelector& PhysicalDeviceSelector::dedicatedComputeQueue(
    bool dedicated)
{
    criteria.dedicatedComputeQueue = dedicated;
    return *this;
}

PhysicalDeviceSelector& PhysicalDeviceSelector::addExtensions(
    const std::vector<const char*>& extensions)
{
    for (const auto& extension : extensions)
    {
        criteria.extensions.push_back(extension);
    }
    return *this;
}

PhysicalDeviceSelector& PhysicalDeviceSelector::setFeatures(
    const VkPhysicalDeviceFeatures& features)
{
    criteria.features = features;
    return *this;
}

PhysicalDeviceSelector& PhysicalDeviceSelector::setFeatures2(
    const VkPhysicalDeviceFeatures2& features)
{
    criteria.features2 = features;
    return *this;
}
}
