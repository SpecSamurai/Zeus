#include "SwapchainBuilder.hpp"

#include "logging/logger.hpp"
#include "vulkan_debug.hpp"
#include "vulkan_device.hpp"
#include "vulkan_memory.hpp"

#include <vulkan/vulkan_core.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <optional>
#include <string>

namespace Zeus
{
void destroySwapchain(const Device& device, const Swapchain& swapchain)
{
    for (auto& imageView : swapchain.imageViews)
    {
        vkDestroyImageView(
            device.logicalDevice,
            imageView,
            allocationCallbacks.get());
    }

    vkDestroySwapchainKHR(
        device.logicalDevice,
        swapchain.handle,
        allocationCallbacks.get());
}

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
        LOG_ERROR("Image usage not supported.");
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
    VkResult result{ vkCreateSwapchainKHR(
        info.device,
        &createInfo,
        allocationCallbacks.get(),
        &swapchain.handle) };

    VKCHECK(result, "Failed to create swap chain.");

    swapchain.imageFormat = surfaceFormat.format;
    swapchain.extent = extent;
    swapchain.imageUsageFlags = info.imageUsageFlags;

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

    swapchain.maxConcurrentFrames = swapchain.imageCount - 1;
    swapchain.imageViews.resize(swapchain.imageCount);

    for (std::uint32_t i{ 0 }; i < swapchain.images.size(); ++i)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.image = swapchain.images[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapchain.imageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        VKCHECK(
            vkCreateImageView(
                info.device,
                &createInfo,
                allocationCallbacks.get(),
                &swapchain.imageViews[i]),
            "Failed to create swapchain image view.");

#ifndef NDEBUG
        std::string imageName{ "Image Swapchain " + std::to_string(i) };
        setDebugUtilsObjectNameEXT(
            info.device,
            VK_OBJECT_TYPE_IMAGE,
            reinterpret_cast<std::uint64_t>(swapchain.images[i]),
            imageName.c_str());

        std::string imageViewName{ "Image View Swapchain " +
                                   std::to_string(i) };
        setDebugUtilsObjectNameEXT(
            info.device,
            VK_OBJECT_TYPE_IMAGE_VIEW,
            reinterpret_cast<std::uint64_t>(swapchain.imageViews[i]),
            imageViewName.c_str());
#endif
    }

    LOG_DEBUG("Swapchain created images count {}", swapchain.imageCount);
    LOG_DEBUG("Max Concurrent Frames {}.", swapchain.maxConcurrentFrames);

    assert(swapchain.imageCount == imageCount);
    assert(swapchain.images.size() == imageCount);

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
        isValid = false;
        LOG_ERROR("Device not set.");
    }

    if (info.physicalDevice == VK_NULL_HANDLE)
    {
        isValid = false;
        LOG_ERROR("Physical device not set.");
    }

    if (info.surface == VK_NULL_HANDLE)
    {
        isValid = false;
        LOG_ERROR("Surface not set.");
    }

    if (info.graphicsFamily == UINT32_MAX)
    {
        isValid = false;
        LOG_ERROR("Graphics family not set.");
    }

    if (info.presentFamily == UINT32_MAX)
    {
        isValid = false;
        LOG_ERROR("Present family not set.");
    }

    if (info.desiredExtent.width == 0 || info.desiredExtent.height == 0)
    {
        isValid = false;
        LOG_ERROR("Extent not set.");
    }

    if (info.desiredSurfaceFormats.empty())
    {
        VkSurfaceFormatKHR surfaceFormatUNORM{
            VK_FORMAT_B8G8R8A8_UNORM,
            VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        };

        VkSurfaceFormatKHR surfaceFormatSRGB{
            VK_FORMAT_B8G8R8A8_SRGB,
            VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        };

        info.desiredSurfaceFormats.emplace_back(surfaceFormatUNORM);
        info.desiredSurfaceFormats.emplace_back(surfaceFormatSRGB);
        LOG_WARNING("Desired formats not set. Used default.");
    }

    return isValid;
}

void SwapchainBuilder::setDevice(
    const Device& device,
    const VkSurfaceKHR& surface)
{
    info.device = device.logicalDevice;
    info.physicalDevice = device.physicalDevice;
    info.surface = surface;

    info.graphicsFamily = device.graphicsFamily;
    info.presentFamily = device.presentFamily;
}

void SwapchainBuilder::setOldSwapchain(VkSwapchainKHR oldSwapchain)
{
    info.oldSwapchain = oldSwapchain;
}

void SwapchainBuilder::setDesiredExtent(
    std::uint32_t width,
    std::uint32_t height)
{
    info.desiredExtent = VkExtent2D{
        .width = width,
        .height = height,
    };
}

void SwapchainBuilder::setMinImageCount(std::uint32_t minImageCount)
{
    info.minImageCount = minImageCount;
}

void SwapchainBuilder::addDesiredSurfaceFormat(VkSurfaceFormatKHR format)
{
    info.desiredSurfaceFormats.push_back(format);
}

void SwapchainBuilder::setDesiredPresentMode(VkPresentModeKHR presentMode)
{
    info.desiredPresentMode = presentMode;
}

void SwapchainBuilder::setImageUsageFlags(VkImageUsageFlags imageUsageFlags)
{
    info.imageUsageFlags = imageUsageFlags;
}

void SwapchainBuilder::setPreTransformFlags(
    VkSurfaceTransformFlagBitsKHR preTransform)
{
    info.preTransform = preTransform;
}

void SwapchainBuilder::setCompositeAlphaFlags(
    VkCompositeAlphaFlagBitsKHR compositeAlphaFlags)
{
    info.compositeAlpha = compositeAlphaFlags;
}

void SwapchainBuilder::setImageArrayLayerCount(std::uint32_t arrayLayerCount)
{
    info.arrayLayerCount = arrayLayerCount;
}
}
