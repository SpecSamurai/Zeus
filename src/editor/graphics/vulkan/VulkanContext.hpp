#pragma once

#include "glfw_utils.hpp"
#include "vulkan_debug.hpp"
#include "vulkan_device.hpp"
#include "vulkan_instance.hpp"
#include "vulkan_surface.hpp"
#include "vulkan_swapchain.hpp"
#include "vulkan_utils.hpp"

#include <core/logger.hpp>

#include <vulkan/vulkan_core.h>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace Zeus
{
class VulkanContext
{
public:
    void Init()
    {
        window = createGlfwWindow(800, 600, "Zeus");
        createVkInstance(instance);
#ifndef NDEBUG
        createVkDebugUtilsMessengerEXT(instance, debugUtilsMessenger);
#endif
        createVkSurfaceKHR(instance, window, surface);
        createVulkanDevice(instance, surface, device);
        // createVulkanSwapchain(device, surface, window, swapchain);
    }

    void Destroy()
    {
        info("Terminating VulkanContext");

        // for (auto imageView : swapchain.swapChainImageViews)
        // {
        //     vkDestroyImageView(device.logicalDevice, imageView, nullptr);
        // }
        //
        // vkDestroySwapchainKHR(device.logicalDevice, swapchain.handle,
        // nullptr);

        if (device.logicalDevice != VK_NULL_HANDLE)
        {
            vkDestroyDevice(device.logicalDevice, nullptr);
        }

        vkDestroySurfaceKHR(instance, surface, nullptr);
#ifndef NDEBUG
        destroyDebugUtilsMessengerEXT(instance, debugUtilsMessenger, nullptr);
#endif
        vkDestroyInstance(instance, nullptr);

        info("Terminating window");

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void MainLoop()
    {
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
        }

        vkDeviceWaitIdle(device.logicalDevice);
    }

private:
public:
    GLFWwindow* window{ nullptr };
    VkInstance instance{ VK_NULL_HANDLE };
#ifndef NDEBUG
    VkDebugUtilsMessengerEXT debugUtilsMessenger{ VK_NULL_HANDLE };
#endif
    VkSurfaceKHR surface{ VK_NULL_HANDLE };

    VulkanDevice device;
    // VkPhysicalDevice physicalDevice{VK_NULL_HANDLE};
    // VkDevice device{VK_NULL_HANDLE};

    VulkanSwapchain swapchain;
    // VkSwapchainKHR swapChain{VK_NULL_HANDLE};
    // std::vector<VkImage> swapChainImages;
    // VkFormat swapChainImageFormat;
    // VkExtent2D swapChainExtent;
    // std::vector<VkImageView> swapChainImageViews;
    // std::vector<VkFramebuffer> swapChainFramebuffers;
};
}
