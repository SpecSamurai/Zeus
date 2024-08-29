#pragma once

#include "Device.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <optional>
#include <vector>

namespace Zeus
{
struct Swapchain
{
    VkSwapchainKHR handle;

    std::uint32_t imageCount;
    std::uint32_t maxConcurrentFrames;

    VkFormat imageFormat;
    VkImageUsageFlags imageUsageFlags;

    VkExtent2D extent;

    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
};

void destroySwapchain(const Device& device, const Swapchain& swapchain);

class SwapchainBuilder
{
public:
    std::optional<Swapchain> build();

    void setDevice(const Device& device, const VkSurfaceKHR& surface);
    void setOldSwapchain(VkSwapchainKHR oldSwapchain);
    void setDesiredExtent(std::uint32_t width, std::uint32_t height);
    void addDesiredSurfaceFormat(VkSurfaceFormatKHR format);
    void setDesiredPresentMode(VkPresentModeKHR presentMode);
    void setImageUsageFlags(VkImageUsageFlags usageFlags);
    void setImageArrayLayerCount(std::uint32_t arrayLayerCount);
    void setMinImageCount(std::uint32_t minImageCount);
    void setPreTransformFlags(VkSurfaceTransformFlagBitsKHR preTransformFlags);
    void setCompositeAlphaFlags(
        VkCompositeAlphaFlagBitsKHR compositeAlphaFlags);

public:
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
