#include "Swapchain.hpp"

#include "api/vulkan_device.hpp"
#include "vulkan/VkContext.hpp"
#include <vulkan/vulkan_core.h>

namespace Zeus
{
void Swapchain2::Create()
{
    SurfaceDetails surfaceDetails{
        getSurfaceDetails(VkContext::GetDevice().GetPhysicalDevice(), m_surface)
    };

    VkSurfaceFormatKHR surfaceFormatUNORM{ VK_FORMAT_B8G8R8A8_UNORM,
                                           VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

    VkSurfaceFormatKHR surfaceFormatSRGB{ VK_FORMAT_B8G8R8A8_SRGB,
                                          VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

    std::vector<VkSurfaceFormatKHR> desiredSurfaceFormats{};
    desiredSurfaceFormats.emplace_back(surfaceFormatUNORM);
    desiredSurfaceFormats.emplace_back(surfaceFormatSRGB);

    VkSurfaceFormatKHR surfaceFormat{
        selectSurfaceFormat(surfaceDetails.formats, desiredSurfaceFormats)
    };

    VkPresentModeKHR desiredPresentMode{ VK_PRESENT_MODE_MAILBOX_KHR };
    m_presentMode =
        selectPresentMode(surfaceDetails.presentModes, desiredPresentMode);

    VkExtent2D extent{ selectExtent(surfaceDetails.capabilities, m_extent) };

    std::uint32_t minImageCount{ 0 };
    std::uint32_t imageCount{
        minImageCount <= surfaceDetails.capabilities.minImageCount
            // One more image for triple buffering or for more frames in flight
            ? surfaceDetails.capabilities.minImageCount + 1
            : minImageCount
    };

    // 0 is a special value that means that there is no maximum
    if (surfaceDetails.capabilities.maxImageCount > 0 &&
        imageCount > surfaceDetails.capabilities.maxImageCount)
    {
        imageCount = surfaceDetails.capabilities.maxImageCount;
    }

    VkImageUsageFlags imageUsageFlags{ VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                       VK_IMAGE_USAGE_TRANSFER_DST_BIT };

    if ((surfaceDetails.capabilities.supportedUsageFlags & imageUsageFlags) ==
        0)
    {
        LOG_ERROR("Image usage not supported.");
        return;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.presentMode = m_presentMode;
    createInfo.imageExtent = extent;
    std::uint32_t arrayLayerCount{ 1 };
    createInfo.imageArrayLayers = arrayLayerCount;

    VkCompositeAlphaFlagBitsKHR compositeAlpha{
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR
    };
    createInfo.compositeAlpha = compositeAlpha;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = nullptr; // info.oldSwapchain;
    createInfo.imageUsage = imageUsageFlags;

    std::array<std::uint32_t, 2> queueFamilyIndices{
        VkContext::GetDevice().GetQueueFamily(QueueType::Graphics),
        VkContext::GetDevice().GetQueueFamily(QueueType::Present),
    };

    if (VkContext::GetDevice().GetQueueFamily(QueueType::Graphics) !=
        VkContext::GetDevice().GetQueueFamily(QueueType::Present))
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = queueFamilyIndices.size();
        createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    }
    else
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkSurfaceTransformFlagBitsKHR preTransform{
        VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
    };

    if (surfaceDetails.capabilities.supportedTransforms & preTransform)
        createInfo.preTransform = preTransform;
    else
        createInfo.preTransform = surfaceDetails.capabilities.currentTransform;

    VkResult result{ vkCreateSwapchainKHR(
        VkContext::GetLogicalDevice(),
        &createInfo,
        allocationCallbacks.get(),
        &m_handle) };

    VKCHECK(result, "Failed to create swap chain.");

    m_imageFormat = surfaceFormat.format;
    m_extent = extent;
    m_imageUsageFlags = imageUsageFlags;

    vkGetSwapchainImagesKHR(
        VkContext::GetLogicalDevice(),
        m_handle,
        &m_imageCount,
        nullptr);

    m_images.resize(m_imageCount);

    vkGetSwapchainImagesKHR(
        VkContext::GetLogicalDevice(),
        m_handle,
        &m_imageCount,
        m_images.data());

    m_maxConcurrentFrames = m_imageCount - 1;
    m_imageViews.resize(m_imageCount);

    for (std::uint32_t i{ 0 }; i < m_images.size(); ++i)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.image = m_images[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_imageFormat;
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
                VkContext::GetLogicalDevice(),
                &createInfo,
                allocationCallbacks.get(),
                &m_imageViews[i]),
            "Failed to create swapchain image view.");

#ifndef NDEBUG
        std::string imageName{ "Image Swapchain " + std::to_string(i) };
        setDebugUtilsObjectNameEXT(
            VkContext::GetLogicalDevice(),
            VK_OBJECT_TYPE_IMAGE,
            reinterpret_cast<std::uint64_t>(m_images[i]),
            imageName.c_str());

        std::string imageViewName{ "Image View Swapchain " +
                                   std::to_string(i) };
        setDebugUtilsObjectNameEXT(
            VkContext::GetLogicalDevice(),
            VK_OBJECT_TYPE_IMAGE_VIEW,
            reinterpret_cast<std::uint64_t>(m_imageViews[i]),
            imageViewName.c_str());
#endif
    }

    LOG_DEBUG("Swapchain created images count {}", m_imageCount);
    LOG_DEBUG("Max Concurrent Frames {}.", m_maxConcurrentFrames);

    assert(m_imageCount == imageCount);
    assert(m_images.size() == imageCount);
}

VkSurfaceFormatKHR Swapchain2::selectSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& surfaceFormats,
    const std::vector<VkSurfaceFormatKHR>& desiredFormats)
{
    // If the list contains only one entry with undefined format it means
    // that there are no preferred surface formats and any can be chosen.
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

VkPresentModeKHR Swapchain2::selectPresentMode(
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

VkExtent2D Swapchain2::selectExtent(
    const VkSurfaceCapabilitiesKHR& surfaceCapabilities,
    const VkExtent2D& desiredExtent)
{
    // When this is true then vulkan tells us that we have to use this(match
    // the resolution of the window)
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

}
