#pragma once

#include "PhysicalDevice.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <optional>
#include <vector>

namespace Zeus
{
struct PhysicalDeviceSelectorInfo
{
    std::vector<const char*> extensions;
    VkInstance instance;
    VkSurfaceKHR surface;

    VkPhysicalDeviceType preferredType;

    VkPhysicalDeviceFeatures2 features2;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceVulkan12Features features1_2;
    VkPhysicalDeviceVulkan13Features features1_3;

    bool requirePresent;
    bool dedicatedTransferQueue;
    bool dedicatedComputeQueue;
};

struct SelectedQueueFamiliesInfo
{
    std::optional<std::uint32_t> graphicsFamily;
    std::optional<std::uint32_t> presentFamily;
    std::optional<std::uint32_t> transferFamily;
    std::optional<std::uint32_t> computeFamily;
};

class PhysicalDeviceSelector
{
public:
    static std::optional<PhysicalDevice> Select(
        const PhysicalDeviceSelectorInfo& info);

private:
    static bool Validate(const PhysicalDeviceSelectorInfo& info);

    static std::optional<PhysicalDevice> CreateIfValid(
        const PhysicalDeviceSelectorInfo& info,
        const VkPhysicalDevice& physicalDevice);

    static std::int32_t RatePhysicalDevice(
        const PhysicalDeviceSelectorInfo& info,
        const PhysicalDevice& physicalDevice);

    static bool CheckPhysicalDeviceFeatures(
        const PhysicalDeviceSelectorInfo& info,
        VkPhysicalDevice physicalDevice);
};
}
