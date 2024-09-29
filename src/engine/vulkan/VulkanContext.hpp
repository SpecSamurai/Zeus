#pragma once

#include "Device.hpp"
#include "Instance.hpp"
#include "SwapchainBuilder.hpp"
#include "vulkan_memory.hpp"
#include "window/Window.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>

namespace Zeus
{
class VulkanContext
{
public:
    VulkanContext(std::uint32_t width, std::uint32_t height, const char* title);

    void Init();
    void Destroy();
    void ResizeSwapchain(const VkExtent2D& extent);

private:
    void InitInstance();
    void InitDevice();
    void InitMemoryAllocator();
    void InitSwapchain();

public:
    SwapchainBuilder swapchainBuilder;
    Window window;
    Instance instance;
    Device device;
    VmaAllocator allocator{ VK_NULL_HANDLE };
    VkSurfaceKHR surface{ VK_NULL_HANDLE };
    Swapchain swapchain;
};
}
