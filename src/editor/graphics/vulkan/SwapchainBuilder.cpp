#include "SwapchainBuilder.hpp"

#include "vulkan_device.hpp"
#include "vulkan_image.hpp"
#include "vulkan_settings.hpp"
#include "vulkan_utils.hpp"

#include <core/logger.hpp>

#include <vulkan/vulkan_core.h>

#include <algorithm>
#include <array>
#include <optional>

namespace Zeus
{
VkSurfaceFormatKHR selectSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& surfaceFormats,
    const std::vector<VkSurfaceFormatKHR>& desiredFormats);

VkPresentModeKHR selectPresentMode(
    const std::vector<VkPresentModeKHR>& presentModes,
    VkPresentModeKHR desiredPresentMode);

VkExtent2D selectExtent(
    const VkSurfaceCapabilitiesKHR& surfaceCapabilities,
    const VkExtent2D& desiredExtent);

std::optional<Swapchain> SwapchainBuilder::build()
{
    if (!validate())
        return std::nullopt;

    SurfaceDetails surfaceDetails{
        getSurfaceDetails(info.physicalDevice, info.surface)
    };

    VkSurfaceFormatKHR surfaceFormat{
        selectSurfaceFormat(surfaceDetails.formats, info.desiredSurfaceFormats)
    };

    VkPresentModeKHR presentMode{
        selectPresentMode(surfaceDetails.presentModes, info.desiredPresentMode)
    };

    VkExtent2D extent{
        selectExtent(surfaceDetails.capabilities, info.desiredExtent)
    };

    std::uint32_t imageCount{
        info.minImageCount <= surfaceDetails.capabilities.minImageCount
            // One more image for triple buffering or for more frames in flight
            ? surfaceDetails.capabilities.minImageCount + 1
            : info.minImageCount
    };

    // 0 is a special value that means that there is no maximum
    if (surfaceDetails.capabilities.maxImageCount > 0 &&
        imageCount > surfaceDetails.capabilities.maxImageCount)
    {
        imageCount = surfaceDetails.capabilities.maxImageCount;
    }

    if ((surfaceDetails.capabilities.supportedUsageFlags &
         info.imageUsageFlags) == 0)
    {
        error("Image usage not supported.");
        return std::nullopt;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = info.surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.presentMode = presentMode;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = info.arrayLayerCount;
    createInfo.compositeAlpha = info.compositeAlpha;
    createInfo.clipped = info.clipped;
    createInfo.oldSwapchain = info.oldSwapchain;
    createInfo.imageUsage = info.imageUsageFlags;

    std::array<std::uint32_t, 2> queueFamilyIndices{ info.graphicsFamily,
                                                     info.presentFamily };

    if (info.graphicsFamily != info.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = queueFamilyIndices.size();
        createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    }
    else
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (surfaceDetails.capabilities.supportedTransforms & info.preTransform)
        createInfo.preTransform = info.preTransform;
    else
        createInfo.preTransform = surfaceDetails.capabilities.currentTransform;

    Swapchain swapchain{};

    VkResult result{
        vkCreateSwapchainKHR(
            info.device,
            &createInfo,
            nullptr,
            &swapchain.handle),
    };

    if (result != VK_SUCCESS)
    {
        error("Failed to create swap chain. {}", vkResultToString(result));
        return std::nullopt;
    }

    swapchain.imageCount = imageCount;
    swapchain.colorSpace = surfaceFormat.colorSpace;
    swapchain.imageFormat = surfaceFormat.format;
    swapchain.extent = extent;
    swapchain.presentMode = presentMode;

    vkGetSwapchainImagesKHR(
        info.device,
        swapchain.handle,
        &swapchain.imageCount,
        nullptr);

    swapchain.images.resize(swapchain.imageCount);

    vkGetSwapchainImagesKHR(
        info.device,
        swapchain.handle,
        &swapchain.imageCount,
        swapchain.images.data());

    swapchain.imageViews.resize(swapchain.images.size());

    for (std::uint32_t i{ 0 }; i < swapchain.images.size(); ++i)
    {
        if (!createVkImageView(
                info.device,
                swapchain.images[i],
                swapchain.imageFormat,
                VK_IMAGE_ASPECT_COLOR_BIT,
                1,
                swapchain.imageViews[i]))
        {
            return std::nullopt;
        }
    }

    return swapchain;
}

VkSurfaceFormatKHR selectSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& surfaceFormats,
    const std::vector<VkSurfaceFormatKHR>& desiredFormats)
{
    // If the list contains only one entry with undefined format it means that
    // there are no preferred surface formats and any can be chosen.
    // Can't use format from one pair and colorspace from another pair.
    if ((surfaceFormats.size() == 1) &&
        (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
    {
        return desiredFormats[0];
    }

    for (const auto& desiredFormat : desiredFormats)
    {
        for (const auto& surfaceFormat : surfaceFormats)
        {
            if (surfaceFormat.format == desiredFormat.format &&
                surfaceFormat.colorSpace == desiredFormat.colorSpace)
            {
                return surfaceFormat;
            }
        }
    }

    return surfaceFormats[0];
}

VkPresentModeKHR selectPresentMode(
    const std::vector<VkPresentModeKHR>& presentModes,
    VkPresentModeKHR desiredPresentMode)
{
    for (const auto& presentMode : presentModes)
    {
        if (presentMode == desiredPresentMode)
        {
            return presentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D selectExtent(
    const VkSurfaceCapabilitiesKHR& surfaceCapabilities,
    const VkExtent2D& desiredExtent)
{
    // When this is true then vulkan tells us that we have to use this(match the
    // resolution of the window)
    if (surfaceCapabilities.currentExtent.width !=
        std::numeric_limits<std::uint32_t>::max())
    {
        return surfaceCapabilities.currentExtent;
    }
    // if false then it tells us that this window manager allow us to differ
    else
    {
        VkExtent2D actualExtent{};

        actualExtent.width = std::clamp(
            desiredExtent.width,
            surfaceCapabilities.minImageExtent.width,
            surfaceCapabilities.maxImageExtent.width);

        actualExtent.height = std::clamp(
            desiredExtent.height,
            surfaceCapabilities.minImageExtent.height,
            surfaceCapabilities.maxImageExtent.height);

        return actualExtent;
    }
}

bool SwapchainBuilder::validate()
{
    bool isValid{ true };

    if (info.device == VK_NULL_HANDLE)
    {
        error("Device not set.");
        isValid = false;
    }

    if (info.physicalDevice == VK_NULL_HANDLE)
    {
        error("Physical device not set.");
        isValid = false;
    }

    if (info.surface == VK_NULL_HANDLE)
    {
        error("Surface not set.");
        isValid = false;
    }

    if (info.graphicsFamily == UINT32_MAX)
    {
        error("Graphics family not set.");
        isValid = false;
    }

    if (info.presentFamily == UINT32_MAX)
    {
        error("Present family not set.");
        isValid = false;
    }

    if (info.desiredExtent.width == 0 || info.desiredExtent.height == 0)
    {
        error("Extent not set.");
        isValid = false;
    }

    if (info.desiredSurfaceFormats.empty())
    {
        info.desiredSurfaceFormats.push_back(INSTANCE_DEFAULT.SURFACE_FORMAT);
        warning("Desired formats not set. Used default.");
    }

    return isValid;
}

SwapchainBuilder& SwapchainBuilder::setDevice(
    const Device& device,
    const VkSurfaceKHR& surface)
{
    info.device = device.logicalDevice;
    info.physicalDevice = device.physicalDevice;
    info.surface = surface;

    info.graphicsFamily = device.graphicsFamily;
    info.presentFamily = device.presentFamily;

    return *this;
}

SwapchainBuilder& SwapchainBuilder::setOldSwapchain(VkSwapchainKHR oldSwapchain)
{
    info.oldSwapchain = oldSwapchain;
    return *this;
}

SwapchainBuilder& SwapchainBuilder::setDesiredExtent(
    std::uint32_t width,
    std::uint32_t height)
{
    info.desiredExtent = VkExtent2D{
        .width = width,
        .height = height,
    };

    return *this;
}

SwapchainBuilder& SwapchainBuilder::setMinImageCount(
    std::uint32_t minImageCount)
{
    info.minImageCount = minImageCount;
    return *this;
}

SwapchainBuilder& SwapchainBuilder::addDesiredSurfaceFormat(
    VkSurfaceFormatKHR format)
{
    info.desiredSurfaceFormats.push_back(format);
    return *this;
}

SwapchainBuilder& SwapchainBuilder::setDesiredPresentMode(
    VkPresentModeKHR presentMode)
{
    info.desiredPresentMode = presentMode;
    return *this;
}

SwapchainBuilder& SwapchainBuilder::setImageUsageFlags(
    VkImageUsageFlags imageUsageFlags)
{
    info.imageUsageFlags = imageUsageFlags;
    return *this;
}

SwapchainBuilder& SwapchainBuilder::setPreTransformFlags(
    VkSurfaceTransformFlagBitsKHR preTransform)
{
    info.preTransform = preTransform;
    return *this;
}

SwapchainBuilder& SwapchainBuilder::setCompositeAlphaFlags(
    VkCompositeAlphaFlagBitsKHR compositeAlphaFlags)
{
    info.compositeAlpha = compositeAlphaFlags;
    return *this;
}

SwapchainBuilder& SwapchainBuilder::setImageArrayLayerCount(
    std::uint32_t arrayLayerCount)
{
    info.arrayLayerCount = arrayLayerCount;
    return *this;
}
}
