#pragma once

#include "Device.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <optional>
#include <vector>

namespace Zeus
{
struct Swapchain
{
    // VkDevice device{ VK_NULL_HANDLE };
    VkSwapchainKHR handle{ VK_NULL_HANDLE };

    std::uint32_t imageCount{ 0 };
    // u8 max_frames_in_flight;

    VkFormat imageFormat;
    VkColorSpaceKHR colorSpace;
    // VkImageUsageFlags imageUsageFlags = 0;

    VkExtent2D extent;

    VkPresentModeKHR presentMode;

    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
    // std::vector<VkFramebuffer> swapchainFramebuffers;
};

inline void destroySwapchain(const Device& device, const Swapchain& swapchain)
{
    for (auto& imageView : swapchain.imageViews)
    {
        vkDestroyImageView(device.logicalDevice, imageView, nullptr);
    }

    // vkDestroySwapchainKHR(device.logicalDevice, swapchain.handle, nullptr);
}

class SwapchainBuilder
{
public:
    std::optional<Swapchain> build();

    SwapchainBuilder& setDevice(
        const Device& device,
        const VkSurfaceKHR& surface);

    SwapchainBuilder& setOldSwapchain(VkSwapchainKHR oldSwapchain);

    SwapchainBuilder& setDesiredExtent(
        std::uint32_t width,
        std::uint32_t height);

    SwapchainBuilder& addDesiredSurfaceFormat(VkSurfaceFormatKHR format);

    SwapchainBuilder& setDesiredPresentMode(VkPresentModeKHR presentMode);

    SwapchainBuilder& setImageUsageFlags(VkImageUsageFlags usageFlags);

    SwapchainBuilder& setImageArrayLayerCount(std::uint32_t arrayLayerCount);

    SwapchainBuilder& setMinImageCount(std::uint32_t minImageCount);

    // Default is no transform.
    SwapchainBuilder& setPreTransformFlags(
        VkSurfaceTransformFlagBitsKHR preTransformFlags);

    SwapchainBuilder& setCompositeAlphaFlags(
        VkCompositeAlphaFlagBitsKHR compositeAlphaFlags);

private:
    bool validate();

    struct SwapchainInfo
    {
        VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };
        VkDevice device{ VK_NULL_HANDLE };
        VkSurfaceKHR surface{ VK_NULL_HANDLE };

        std::uint32_t graphicsFamily{ UINT32_MAX };
        std::uint32_t presentFamily{ UINT32_MAX };

        VkSwapchainKHR oldSwapchain{ VK_NULL_HANDLE };

        VkExtent2D desiredExtent{};

        std::uint32_t minImageCount{ 0 };
        std::uint32_t required_min_image_count{ 0 };

        std::vector<VkSurfaceFormatKHR> desiredSurfaceFormats{};

        VkPresentModeKHR desiredPresentMode{ VK_PRESENT_MODE_MAILBOX_KHR };

        VkImageUsageFlags imageUsageFlags{
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
        };

        VkSurfaceTransformFlagBitsKHR preTransform{
            VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
        };

        VkCompositeAlphaFlagBitsKHR compositeAlpha{
            VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR
        };

        // always 1 unless you are developing a stereoscopic 3D application
        std::uint32_t arrayLayerCount{ 1 };
        VkBool32 clipped{ VK_TRUE };
    } info;
};
}
