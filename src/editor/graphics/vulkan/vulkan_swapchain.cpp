#include "vulkan_swapchain.hpp"

#include "vulkan_device.hpp"
#include "vulkan_image.hpp"
#include "vulkan_settings.hpp"
#include "vulkan_utils.hpp"

#include <core/logger.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include <algorithm>
#include <array>

namespace Zeus
{
bool createVulkanSwapchain(
    const Device& device,
    const VkSurfaceKHR& surface,
    GLFWwindow* window,
    VulkanSwapchain& vulkanSwapchain)
{
    SurfaceDetails surfaceDetails =
        getSurfaceDetails(device.physicalDevice, surface);

    VkSurfaceFormatKHR surfaceFormat =
        selectSurfaceFormat(surfaceDetails.formats);

    VkPresentModeKHR presentMode =
        selectPresentMode(surfaceDetails.presentModes);

    VkExtent2D extent = selectExtent(surfaceDetails.capabilities, window);

    // One more image for triple buffering or simply for more frames in flight
    std::uint32_t imageCount = surfaceDetails.capabilities.minImageCount + 1;
    // 0 is a special value that means that there is no maximum
    if (surfaceDetails.capabilities.maxImageCount > 0 &&
        imageCount > surfaceDetails.capabilities.maxImageCount)
    {
        imageCount = surfaceDetails.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    // we can't use format from one pair and colorspace from another pair
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    // always 1 unless you are developing a stereoscopic 3D application
    createInfo.imageArrayLayers = 1;

    // if (surface_capabilities.supportedUsageFlags &
    //     VK_IMAGE_USAGE_TRANSFER_DST_BIT)
    // {
    //     return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
    //            VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    // }
    // std::cout << "VK_IMAGE_USAGE_TRANSFER_DST image usage is not supported
    // by"
    //              "the swap chain!"
    //           << std::endl
    //           << "Supported swap chain's image usages include:" << std::endl
    //           << (surface_capabilities.supportedUsageFlags &
    //                       VK_IMAGE_USAGE_TRANSFER_SRC_BIT
    //                   ? "    VK_IMAGE_USAGE_TRANSFER_SRC\n"
    //                   : "")
    //           << (surface_capabilities.supportedUsageFlags &
    //                       VK_IMAGE_USAGE_TRANSFER_DST_BIT
    //                   ? "    VK_IMAGE_USAGE_TRANSFER_DST\n"
    //                   : "")
    //           << (surface_capabilities.supportedUsageFlags &
    //                       VK_IMAGE_USAGE_SAMPLED_BIT
    //                   ? "    VK_IMAGE_USAGE_SAMPLED\n"
    //                   : "")
    //           << (surface_capabilities.supportedUsageFlags &
    //                       VK_IMAGE_USAGE_STORAGE_BIT
    //                   ? "    VK_IMAGE_USAGE_STORAGE\n"
    //                   : "")
    //           << (surface_capabilities.supportedUsageFlags &
    //                       VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
    //                   ? "    VK_IMAGE_USAGE_COLOR_ATTACHMENT\n"
    //                   : "")
    //           << (surface_capabilities.supportedUsageFlags &
    //                       VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
    //                   ? "    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT\n"
    //                   : "")
    //           << (surface_capabilities.supportedUsageFlags &
    //                       VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT
    //                   ? "    VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT\n"
    //                   : "")
    //           << (surface_capabilities.supportedUsageFlags &
    //                       VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT
    //                   ? "    VK_IMAGE_USAGE_INPUT_ATTACHMENT"
    //                   : "")
    //           << std::endl;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    std::array<std::uint32_t, 2> queueFamilyIndices{ device.graphicsFamily,
                                                     device.presentFamily };

    if (device.graphicsFamily != device.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = queueFamilyIndices.size();
        createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;     // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    if (surfaceDetails.capabilities.supportedTransforms &
        VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
    {
        createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
    else
    {
        createInfo.preTransform = surfaceDetails.capabilities.currentTransform;
    }

    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    // auto oldSwapchain = vulkanSwapchain.handle;
    // createInfo.oldSwapchain = oldSwapchain;

    VkResult result{
        vkCreateSwapchainKHR(
            device.logicalDevice,
            &createInfo,
            nullptr,
            &vulkanSwapchain.handle),
    };

    if (result != VK_SUCCESS)
    {
        error("Failed to create swap chain. {}", vkResultToString(result));
        return false;
    }

    // if (oldSwapchain != VK_NULL_HANDLE)
    // {
    //     vkDestroySwapchainKHR(
    //         vulkanDevice.logicalDevice,
    //         oldSwapchain,
    //         nullptr);
    // }

    vkGetSwapchainImagesKHR(
        device.logicalDevice,
        vulkanSwapchain.handle,
        &imageCount,
        nullptr);

    vulkanSwapchain.swapchainImages.resize(imageCount);

    vkGetSwapchainImagesKHR(
        device.logicalDevice,
        vulkanSwapchain.handle,
        &imageCount,
        vulkanSwapchain.swapchainImages.data());

    vulkanSwapchain.imageFormat = surfaceFormat.format;
    vulkanSwapchain.extent = extent;

    vulkanSwapchain.swapchainImageViews.resize(
        vulkanSwapchain.swapchainImages.size());

    for (std::uint32_t i{ 0 }; i < vulkanSwapchain.swapchainImages.size(); ++i)
    {
        if (!createVkImageView(
                device.logicalDevice,
                vulkanSwapchain.swapchainImages[i],
                vulkanSwapchain.imageFormat,
                VK_IMAGE_ASPECT_COLOR_BIT,
                1,
                vulkanSwapchain.swapchainImageViews[i]))
        {
            return false;
        }
    }

    return true;
}

VkSurfaceFormatKHR selectSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& surfaceFormats)
{
    // // If the list contains only one entry with undefined format
    // // it means that there are no preferred surface formats and any can be
    // // chosen
    // if ((surface_formats.size() == 1) &&
    //     (surface_formats[0].format == VK_FORMAT_UNDEFINED))
    // {
    //     return { VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR
    //     };
    // }
    //
    // // Check if list contains most widely used R8 G8 B8 A8 format
    // // with nonlinear color space
    // for (VkSurfaceFormatKHR& surface_format : surface_formats)
    // {
    //     if (surface_format.format == VK_FORMAT_R8G8B8A8_UNORM)
    //     {
    //         return surface_format;
    //     }
    // }
    //
    // return surface_formats[0];
    for (const auto& surfaceFormat : surfaceFormats)
    {
        // If the list contains only one entry with undefined format
        // it means that there are no preferred surface formats and any can
        // be chosen
        if (surfaceFormat.format ==
                VK_FORMAT_R8G8B8A8_SRGB && // VK_FORMAT_B8G8R8A8_SRGB &&
            surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return surfaceFormat;
        }
    }

    return surfaceFormats[0];
}

VkPresentModeKHR selectPresentMode(
    const std::vector<VkPresentModeKHR>& presentModes)
{
    if (!VSYNC)
    {
        return VK_PRESENT_MODE_IMMEDIATE_KHR;
    }

    for (const auto& presentMode : presentModes)
    {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return presentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D selectExtent(
    const VkSurfaceCapabilitiesKHR& surfaceCapabilities,
    GLFWwindow* window)
{
    // when this is true then vulkan tells us(by setting currentExten)
    // that we have to use this(match the resolution of the window)
    // *** Special value of surface extent is width == height == -1
    // If this is so we define the size by ourselves but it must fit within
    // defined confines
    if (surfaceCapabilities.currentExtent.width !=
        std::numeric_limits<std::uint32_t>::max())
    {
        return surfaceCapabilities.currentExtent;
    }
    // if false then it tells us that this window manager allow us to differ
    else
    {
        // WIDTH and HEIGH are set in GLFW screen coordinates
        // which might not corresponde to pixel coordinates due to higher
        // pixel density
        // in this case Vulkan didnt fixed the swap extend for us
        int width, height;
        // to query the resolution of the window in pixel before matching it
        // against the minimum and maximum image extent
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = { static_cast<std::uint32_t>(width),
                                    static_cast<std::uint32_t>(height) };

        actualExtent.width = std::clamp(
            actualExtent.width,
            surfaceCapabilities.minImageExtent.width,
            surfaceCapabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(
            actualExtent.height,
            surfaceCapabilities.minImageExtent.height,
            surfaceCapabilities.maxImageExtent.height);

        return actualExtent;
    }
}
}
