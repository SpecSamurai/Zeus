#pragma once

#include "Device.hpp"
#include "Instance.hpp"
#include "SwapchainBuilder.hpp"
#include "vulkan_memory.hpp"
#include "window/Window.hpp"

#include <vulkan/vulkan_core.h>

namespace Zeus
{
class VulkanContext
{
public:
    VulkanContext(const Window& window);

    void Init();
    void Destroy();
    void ResizeSwapchain(const VkExtent2D& extent);

    Instance& GetInstance();
    Device& GetDevice();
    VmaAllocator& GetAllocator();
    Swapchain& GetSwapchain();

private:
    void InitInstance();
    void InitDevice();
    void InitMemoryAllocator();
    void InitSwapchain();

private:
    const Window& m_window;
    SwapchainBuilder m_swapchainBuilder;
    Instance m_instance;
    Device m_device;
    VmaAllocator m_allocator{ VK_NULL_HANDLE };
    VkSurfaceKHR m_surface{ VK_NULL_HANDLE };
    Swapchain m_swapchain;
};
}
