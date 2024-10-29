#include "VkContext.hpp"

#include "Device.hpp"
#include "DeviceBuilder.hpp"
#include "InstanceBuilder.hpp"
#include "api/vulkan_debug.hpp"
#include "api/vulkan_memory.hpp"
#include "api/vulkan_surface.hpp"
#include "logging/logger.hpp"
#include "window/Window.hpp"
#include "window/glfw_utils.hpp"

#include <vulkan/vulkan_core.h>

namespace Zeus
{
VkInstance VkContext::s_instance{ VK_NULL_HANDLE };
VkDebugUtilsMessengerEXT VkContext::s_debugUtilsMessenger{ VK_NULL_HANDLE };
VkSurfaceKHR VkContext::s_surface{ VK_NULL_HANDLE };
Device VkContext::s_device{};
VmaAllocator VkContext::s_allocator{ VK_NULL_HANDLE };

void VkContext::Init(const Window& window)
{
    InitInstance(window);
    InitDevice(window);
    InitMemoryAllocator();
}

void VkContext::Destroy()
{
    LOG_DEBUG("Destroying VulkanContext");

    vmaDestroyAllocator(s_allocator);
    s_allocator = VK_NULL_HANDLE;

    s_device.Destroy();

    vkDestroySurfaceKHR(s_instance, s_surface, allocationCallbacks.get());
    s_surface = VK_NULL_HANDLE;

    if (s_debugUtilsMessenger != VK_NULL_HANDLE)
    {
        destroyDebugUtilsMessengerEXT(
            s_instance,
            s_debugUtilsMessenger,
            allocationCallbacks.get());
        s_debugUtilsMessenger = VK_NULL_HANDLE;
    }

    vkDestroyInstance(s_instance, allocationCallbacks.get());
    s_instance = VK_NULL_HANDLE;
}

const VkInstance& VkContext::GetInstance()
{
    return s_instance;
}

Device& VkContext::GetDevice()
{
    return s_device;
}

const VkDevice& VkContext::GetLogicalDevice()
{
    return s_device.GetLogicalDevice();
}

const VkSurfaceKHR& VkContext::GetSurface()
{
    return s_surface;
}

const VmaAllocator& VkContext::GetAllocator()
{
    return s_allocator;
}

void VkContext::InitInstance(const Window& window)
{
    InstanceBuilder instanceBuilder;
    instanceBuilder.SetApiVersion(VK_API_VERSION_1_3);
    instanceBuilder.SetEngineName("Zeus");
    instanceBuilder.SetEngineVersion(VK_MAKE_VERSION(0, 0, 1));
    instanceBuilder.SetAppName(window.GetTitle());
    instanceBuilder.SetApplicationVersion(VK_MAKE_VERSION(0, 0, 1));
    instanceBuilder.SetExtensions(getRequiredGlobalExtensions());

    Instance instance{ instanceBuilder.Build() };

    s_instance = instance.handle;
    s_debugUtilsMessenger = instance.debugUtilsMessenger;
}

void VkContext::InitDevice(const Window& window)
{
    createVkSurfaceKHR(s_instance, window.GetHandle(), &s_surface);

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

    DeviceBuilder deviceBuilder;
    deviceBuilder.AddExtensions({
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        // VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
        // VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
        // VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
        // VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
    });
    deviceBuilder.SetInstance(s_instance);
    deviceBuilder.SetSurface(s_surface);
    deviceBuilder.SetPreferredType(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
    deviceBuilder.RequirePresent();
    deviceBuilder.DedicatedTransferQueue();
    deviceBuilder.DedicatedComputeQueue();
    deviceBuilder.SetFeatures1_3(features1_3);
    deviceBuilder.SetFeatures1_2(features1_2);
    deviceBuilder.SetFeatures(requestedFeatures);

    DeviceInfo deviceInfo{ deviceBuilder.Build() };
    s_device.Init(deviceInfo);
}

void VkContext::InitMemoryAllocator()
{
    VmaAllocatorCreateInfo createInfo{};
    createInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    createInfo.vulkanApiVersion = VK_API_VERSION_1_3;
    createInfo.physicalDevice = s_device.GetPhysicalDevice();
    createInfo.device = s_device.GetLogicalDevice();
    createInfo.instance = s_instance;
    createInfo.pAllocationCallbacks = allocationCallbacks.get();

    vmaCreateAllocator(&createInfo, &s_allocator);
}
}
