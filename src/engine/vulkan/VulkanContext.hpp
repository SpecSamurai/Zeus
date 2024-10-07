#pragma once

#include "Device.hpp"
#include "Instance.hpp"
#include "SwapchainBuilder.hpp"
#include "vulkan_memory.hpp"
#include "window/Window.hpp"

#include <vulkan/vulkan_core.h>

#include <functional>

namespace Zeus
{
class VulkanContext
{
public:
    VulkanContext(const Window& window);

    void Init();
    void Destroy();
    void ResizeSwapchain(const VkExtent2D& extent);

    void CmdImmediateSubmit(
        std::function<void(VkCommandBuffer cmd)>&& function);

    Instance& GetInstance();
    Device& GetDevice();
    VmaAllocator& GetAllocator();
    Swapchain& GetSwapchain();

private:
    void InitInstance();
    void InitDevice();
    void InitMemoryAllocator();
    void InitSwapchain();
    void InitSyncObjects();
    void InitCommands();

private:
    const Window& m_window;
    SwapchainBuilder m_swapchainBuilder;
    Instance m_instance;
    Device m_device;
    VmaAllocator m_allocator{ VK_NULL_HANDLE };
    VkSurfaceKHR m_surface{ VK_NULL_HANDLE };
    Swapchain m_swapchain;

    VkFence m_ImmediateSubmitFence{ VK_NULL_HANDLE };
    VkCommandPool m_ImmediateSubmitCommandPool{ VK_NULL_HANDLE };
    VkCommandBuffer m_ImmediateSubmitCommandBuffer{ VK_NULL_HANDLE };
};
}
