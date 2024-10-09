#pragma once

#include "Device.hpp"
#include "Instance.hpp"
#include "vulkan_memory.hpp"
#include "window/Window.hpp"

#include <vulkan/vulkan_core.h>

namespace Zeus
{
class VulkanContext
{
public:
    void Init(const Window& window);
    void Destroy();

private:
    void InitInstance(const Window& window);
    void InitDevice(const Window& window);
    void InitMemoryAllocator();

public:
    Instance instance;
    Device device;
    VmaAllocator allocator{ VK_NULL_HANDLE };
    VkSurfaceKHR surface{ VK_NULL_HANDLE };
};
}
