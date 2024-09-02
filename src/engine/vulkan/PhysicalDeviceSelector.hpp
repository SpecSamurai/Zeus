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

    void setSurface(const VkSurfaceKHR& surface);
    void setPreferredType(VkPhysicalDeviceType preferredType);
    void requirePresent(bool require = true);
    void dedicatedTransferQueue(bool dedicated = true);
    void dedicatedComputeQueue(bool dedicated = true);
    void addExtensions(const std::vector<const char*>& extensions);
    void setFeatures(const VkPhysicalDeviceFeatures& features);
    void setFeatures1_2(const VkPhysicalDeviceVulkan12Features& features);
    void setFeatures1_3(const VkPhysicalDeviceVulkan13Features& features);

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

        VkPhysicalDeviceFeatures2 features2{};
        VkPhysicalDeviceFeatures features{};
        VkPhysicalDeviceVulkan12Features features1_2{};
        VkPhysicalDeviceVulkan13Features features1_3{};
    } criteria;
};
}
