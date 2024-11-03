#include "VkContext.hpp"

#include "Device.hpp"
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

    createVkSurfaceKHR(s_instance, window.GetHandle(), &s_surface);
    s_device.Init(s_instance, s_surface);

    VmaAllocatorCreateInfo createInfo{};
    createInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    createInfo.vulkanApiVersion = VK_API_VERSION_1_3;
    createInfo.physicalDevice = s_device.GetPhysicalDevice();
    createInfo.device = s_device.GetLogicalDevice();
    createInfo.instance = s_instance;
    createInfo.pAllocationCallbacks = allocationCallbacks.get();

    vmaCreateAllocator(&createInfo, &s_allocator);
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

VkInstance VkContext::GetInstance()
{
    return s_instance;
}

Device& VkContext::GetDevice()
{
    return s_device;
}

VkDevice VkContext::GetLogicalDevice()
{
    return s_device.GetLogicalDevice();
}

VkSurfaceKHR VkContext::GetSurface()
{
    return s_surface;
}

VmaAllocator VkContext::GetAllocator()
{
    return s_allocator;
}
}
