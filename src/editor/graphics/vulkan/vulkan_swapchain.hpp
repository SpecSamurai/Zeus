#pragma once

#include "Device.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include <vector>

namespace Zeus
{
struct VulkanSwapchain
{
    VkSwapchainKHR handle{ VK_NULL_HANDLE };

    VkFormat imageFormat;
    VkPresentModeKHR presentMode;
    VkExtent2D extent;

    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
    std::vector<VkFramebuffer> swapchainFramebuffers;
};

bool createVulkanSwapchain(
    const Device& device,
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
