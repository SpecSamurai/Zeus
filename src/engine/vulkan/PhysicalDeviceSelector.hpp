#pragma once

#include "PhysicalDevice.hpp"

#include <vulkan/vulkan.h>

#include <optional>
#include <vector>

namespace Zeus
{
class PhysicalDeviceSelector
{
public:
    PhysicalDeviceSelector(const VkInstance& instance);

    std::optional<PhysicalDevice> select();

    PhysicalDeviceSelector& setSurface(const VkSurfaceKHR& surface);

    PhysicalDeviceSelector& setPreferredType(
        VkPhysicalDeviceType preferredType);

    PhysicalDeviceSelector& requirePresent(bool require = true);
    PhysicalDeviceSelector& dedicatedTransferQueue(bool dedicated = true);
    PhysicalDeviceSelector& dedicatedComputeQueue(bool dedicated = true);

    PhysicalDeviceSelector& addExtensions(
        const std::vector<const char*>& extensions);

    PhysicalDeviceSelector& setFeatures(
        const VkPhysicalDeviceFeatures& features);

    PhysicalDeviceSelector& setFeatures2(
        const VkPhysicalDeviceFeatures2& features);

private:
    bool validate();

    std::optional<PhysicalDevice> createIfValid(
        const VkPhysicalDevice& physicalDevice);

    int ratePhysicalDevice(const PhysicalDevice& physicalDevice);

    bool checkPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice);

    VkInstance instance{};
    VkSurfaceKHR surface{ VK_NULL_HANDLE };

    struct SelectionCriteria
    {
        VkPhysicalDeviceType preferredType{
            VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
        };

        bool requirePresent{ false };

        bool dedicatedTransferQueue{ false };
        bool dedicatedComputeQueue{ false };

        std::vector<const char*> extensions{};

        VkPhysicalDeviceFeatures features{};
        VkPhysicalDeviceFeatures2 features2{};
    } criteria;
};
}
