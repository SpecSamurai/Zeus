#pragma once

#include "vulkan_device.hpp"

#include "GLFW/glfw3.h"

namespace Zeus
{
struct VulkanSwapchain
{
    VkSwapchainKHR handle{ VK_NULL_HANDLE };

    VkFormat imageFormat;
    VkPresentModeKHR presentMode;
    VkExtent2D extent;

    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;
};

bool createVulkanSwapchain(
    SurfaceDetails swapChainSupport,
    const VulkanDevice& vulkanDevice,
    const VkSurfaceKHR& surface,
    GLFWwindow* window,
    VulkanSwapchain& vulkanSwapchain);

VkSurfaceFormatKHR selectSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& surfaceFormats);

VkPresentModeKHR selectPresentMode(
    const std::vector<VkPresentModeKHR>& presentModes);

VkExtent2D selectExtent(
    const VkSurfaceCapabilitiesKHR& surfaceCapabilities,
    GLFWwindow* window);
}
