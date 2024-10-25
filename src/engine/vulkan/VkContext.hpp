#pragma once

#include "Device.hpp"
#include "api/vulkan_memory.hpp"
#include "window/Window.hpp"

#include <vulkan/vulkan_core.h>

namespace Zeus
{
class VkContext
{
public:
    static void Init(const Window& window);
    static void Destroy();

    static Device& GetDevice();
    static const VkDevice& GetLogicalDevice();
    static const VkSurfaceKHR& GetSurface();
    static const VmaAllocator& GetAllocator();

private:
    static void InitInstance(const Window& window);
    static void InitDevice(const Window& window);
    static void InitMemoryAllocator();

private:
    static VkInstance s_instance;
    static VkDebugUtilsMessengerEXT s_debugUtilsMessenger;
    static VkSurfaceKHR s_surface;
    static Device s_device;
    static VmaAllocator s_allocator;
};
}
