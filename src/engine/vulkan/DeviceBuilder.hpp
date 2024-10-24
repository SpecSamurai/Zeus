#pragma once

#include <vulkan/vulkan_core.h>

#include <vector>

namespace Zeus
{
struct DeviceInfo
{
    VkDevice logicalDevice;
    VkPhysicalDevice physicalDevice;

    std::uint32_t graphicsFamily;
    std::uint32_t presentFamily;
    std::uint32_t transferFamily;
    std::uint32_t computeFamily;

    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkQueue transferQueue;
    VkQueue computeQueue;
};

class DeviceBuilder
{
public:
    DeviceInfo Build();

    void SetInstance(const VkInstance& instance);
    void SetSurface(const VkSurfaceKHR& surface);
    void SetPreferredType(VkPhysicalDeviceType preferredType);
    void RequirePresent(bool require = true);
    void DedicatedTransferQueue(bool dedicated = true);
    void DedicatedComputeQueue(bool dedicated = true);
    void AddExtensions(const std::vector<const char*>& extensions);
    void SetFeatures(const VkPhysicalDeviceFeatures& features);
    void SetFeatures1_2(const VkPhysicalDeviceVulkan12Features& features);
    void SetFeatures1_3(const VkPhysicalDeviceVulkan13Features& features);

private:
    struct DeviceBuilderInfo
    {
        std::vector<const char*> extensions{};
        VkInstance instance;
        VkSurfaceKHR surface;

        VkPhysicalDeviceType preferredType{
            VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
        };

        VkPhysicalDeviceFeatures2 features2{};
        VkPhysicalDeviceFeatures features{};
        VkPhysicalDeviceVulkan12Features features1_2{};
        VkPhysicalDeviceVulkan13Features features1_3{};

        bool requirePresent{ false };
        bool dedicatedTransferQueue{ false };
        bool dedicatedComputeQueue{ false };
    } m_info;
};
}
