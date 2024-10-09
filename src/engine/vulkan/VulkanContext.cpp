#include "VulkanContext.hpp"

#include "Device.hpp"
#include "DeviceBuilder.hpp"
#include "Instance.hpp"
#include "InstanceBuilder.hpp"
#include "PhysicalDeviceSelector.hpp"
#include "core/logger.hpp"
#include "vulkan_memory.hpp"
#include "vulkan_surface.hpp"
#include "window/Window.hpp"
#include "window/glfw_utils.hpp"

#include <vulkan/vulkan_core.h>

namespace Zeus
{
void VulkanContext::Init(const Window& window)
{
    InitInstance(window);
    InitDevice(window);
    InitMemoryAllocator();
}

void VulkanContext::Destroy()
{
    debug("Destroying VulkanContext");

    vmaDestroyAllocator(allocator);
    destroyDevice(device);
    vkDestroySurfaceKHR(instance.handle, surface, allocationCallbacks.get());
    destroyInstance(instance);
}

void VulkanContext::InitInstance(const Window& window)
{
    InstanceBuilder instanceBuilder;
    instanceBuilder.setApiVersion(VK_API_VERSION_1_3);
    instanceBuilder.setEngineName("Zeus");
    instanceBuilder.setEngineVersion(VK_MAKE_VERSION(0, 0, 1));
    instanceBuilder.setAppName(window.title);
    instanceBuilder.setApplicationVersion(VK_MAKE_VERSION(0, 0, 1));
    instanceBuilder.setExtensions(getRequiredGlobalExtensions());

    instance = instanceBuilder.build();
}

void VulkanContext::InitDevice(const Window& window)
{
    createVkSurfaceKHR(instance.handle, window.handle, surface);

    VkPhysicalDeviceFeatures requestedFeatures{};
    requestedFeatures.sampleRateShading = VK_TRUE;
    requestedFeatures.samplerAnisotropy = VK_TRUE;
    requestedFeatures.fillModeNonSolid = VK_TRUE;

    VkPhysicalDeviceVulkan13Features features1_3{};
    features1_3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features1_3.dynamicRendering = true;
    features1_3.synchronization2 = true;

    VkPhysicalDeviceVulkan12Features features1_2{};
    features1_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features1_2.bufferDeviceAddress = true;
    features1_2.descriptorIndexing = true;
    features1_2.descriptorBindingPartiallyBound = true;
    features1_2.descriptorBindingVariableDescriptorCount = true;
    features1_2.runtimeDescriptorArray = true;

    PhysicalDeviceSelector physicalDeviceSelector(instance.handle);
    physicalDeviceSelector.setSurface(surface);
    physicalDeviceSelector.setPreferredType(
        VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
    physicalDeviceSelector.requirePresent();
    physicalDeviceSelector.dedicatedTransferQueue();
    physicalDeviceSelector.dedicatedComputeQueue();
    physicalDeviceSelector.setFeatures(requestedFeatures);
    physicalDeviceSelector.setFeatures1_2(features1_2);
    physicalDeviceSelector.setFeatures1_3(features1_3);

    auto physicalDevice = physicalDeviceSelector.select().value();

    DeviceBuilder deviceBuilder(physicalDevice);
    device = deviceBuilder.build();
}

void VulkanContext::InitMemoryAllocator()
{
    VmaAllocatorCreateInfo createInfo{};
    createInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    createInfo.vulkanApiVersion = VK_API_VERSION_1_3;
    createInfo.physicalDevice = device.physicalDevice;
    createInfo.device = device.logicalDevice;
    createInfo.instance = instance.handle;
    createInfo.pAllocationCallbacks = allocationCallbacks.get();

    vmaCreateAllocator(&createInfo, &allocator);
}
}
