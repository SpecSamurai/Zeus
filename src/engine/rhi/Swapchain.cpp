#include "Swapchain.hpp"

#include "Definitions.hpp"
#include "VkContext.hpp"
#include "logging/logger.hpp"
#include "rhi/CommandBuffer.hpp"
#include "vulkan/vulkan_command.hpp"
#include "vulkan/vulkan_debug.hpp"
#include "vulkan/vulkan_device.hpp"

#include <vulkan/vulkan_core.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <format>
#include <limits>
#include <vector>

namespace Zeus
{
Swapchain::Swapchain(
    const VkSurfaceKHR surface,
    const std::uint32_t width,
    const std::uint32_t height,
    const VkPresentModeKHR presentMode,
    const std::uint32_t frameCount)
    : m_surface{ surface },
      m_framesCount{ frameCount },
      m_extent{ VkExtent2D{ .width = width, .height = height } },
      m_presentMode{ presentMode }
{
    assert(width > 0 && height > 0);
    assert(
        width <= VkContext::Device().GetLimits().maxImageDimension2D &&
        height <= VkContext::Device().GetLimits().maxImageDimension2D);

    Create();
}

Swapchain::Swapchain(Swapchain&& other) noexcept
    : m_surface{ other.m_surface },
      m_handle{ other.m_handle },
      m_images{ std::move(other.m_images) },
      m_imageViews{ std::move(other.m_imageViews) },
      m_layouts{ std::move(other.m_layouts) },
      m_frameSyncData{ std::move(other.m_frameSyncData) },
      m_imageIndex{ other.m_imageIndex },
      m_imageCount{ other.m_imageCount },
      m_frameIndex{ other.m_frameIndex },
      m_framesCount{ other.m_framesCount },
      m_extent{ other.m_extent },
      m_presentMode{ other.m_presentMode },
      m_imageFormat{ other.m_imageFormat },
      m_resizeRequired{ other.m_resizeRequired }
{
    other.m_surface = VK_NULL_HANDLE;
    other.m_handle = VK_NULL_HANDLE;
}

Swapchain& Swapchain::operator=(Swapchain&& other)
{
    if (this != &other)
    {
        if (m_handle != VK_NULL_HANDLE)
        {
            Destroy();
        }

        m_surface = other.m_surface;
        m_handle = other.m_handle;
        m_images = std::move(other.m_images);
        m_imageViews = std::move(other.m_imageViews);
        m_layouts = std::move(other.m_layouts);
        m_frameSyncData = std::move(other.m_frameSyncData);
        m_imageIndex = other.m_imageIndex;
        m_imageCount = other.m_imageCount;
        m_frameIndex = other.m_frameIndex;
        m_framesCount = other.m_framesCount;
        m_extent = other.m_extent;
        m_presentMode = other.m_presentMode;
        m_imageFormat = other.m_imageFormat;
        m_resizeRequired = other.m_resizeRequired;

        other.m_surface = VK_NULL_HANDLE;
        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}

Swapchain::~Swapchain()
{
    if (m_handle == VK_NULL_HANDLE)
        return;

    Destroy();
}

void Swapchain::Create()
{
    SurfaceDetails surfaceDetails{
        getSurfaceDetails(VkContext::Device().GetPhysicalDevice(), m_surface)
    };

    std::vector<VkSurfaceFormatKHR> desiredSurfaceFormats{
        VkSurfaceFormatKHR{ VK_FORMAT_B8G8R8A8_UNORM,
                            VK_COLOR_SPACE_SRGB_NONLINEAR_KHR },
        VkSurfaceFormatKHR{ VK_FORMAT_B8G8R8A8_SRGB,
                            VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }
    };

    VkSurfaceFormatKHR surfaceFormat{
        selectSurfaceFormat(surfaceDetails.formats, desiredSurfaceFormats)
    };

    m_presentMode =
        selectPresentMode(surfaceDetails.presentModes, m_presentMode);

    m_extent = selectExtent(surfaceDetails.capabilities, m_extent);

    m_imageCount =
        m_framesCount <= surfaceDetails.capabilities.minImageCount
            // One more image for triple buffering or for more frames in flight
            ? surfaceDetails.capabilities.minImageCount + 1
            : m_framesCount + 1;

    // 0 is a special value that means that there is no maximum
    if (surfaceDetails.capabilities.maxImageCount > 0 &&
        m_imageCount > surfaceDetails.capabilities.maxImageCount)
    {
        m_imageCount = surfaceDetails.capabilities.maxImageCount;
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
    createInfo.minImageCount = m_imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.presentMode = m_presentMode;
    createInfo.imageExtent = m_extent;
    // always 1 unless you are developing a stereoscopic 3D application
    createInfo.imageArrayLayers = 1;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = m_handle;
    createInfo.imageUsage = imageUsageFlags;

    std::uint32_t graphicsFamily{
        VkContext::GetQueueFamily(QueueType::Graphics),
    };

    std::uint32_t presentFamily{
        VkContext::GetQueueFamily(QueueType::Present),
    };

    std::array<std::uint32_t, 2> queueFamilyIndices{
        graphicsFamily,
        presentFamily,
    };

    if (graphicsFamily != presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = queueFamilyIndices.size();
        createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    VkSurfaceTransformFlagBitsKHR preTransform{
        VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
    };

    if (surfaceDetails.capabilities.supportedTransforms & preTransform)
        createInfo.preTransform = preTransform;
    else
        createInfo.preTransform = surfaceDetails.capabilities.currentTransform;

    VKCHECK(
        vkCreateSwapchainKHR(
            VkContext::LogicalDevice(),
            &createInfo,
            allocationCallbacks.get(),
            &m_handle),
        "Failed to create swap chain.");

    m_imageFormat = surfaceFormat.format;

    vkGetSwapchainImagesKHR(
        VkContext::LogicalDevice(),
        m_handle,
        &m_imageCount,
        nullptr);

    m_images.resize(m_imageCount);
    m_layouts.resize(m_imageCount);

    vkGetSwapchainImagesKHR(
        VkContext::LogicalDevice(),
        m_handle,
        &m_imageCount,
        m_images.data());

    assert(m_images.size() == m_imageCount);

    m_imageViews.resize(m_imageCount);

    for (std::uint32_t i{ 0 }; i < m_images.size(); ++i)
    {
        m_layouts[i] = VK_IMAGE_LAYOUT_UNDEFINED;

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
                VkContext::LogicalDevice(),
                &createInfo,
                allocationCallbacks.get(),
                &m_imageViews[i]),
            "Failed to create swapchain image view.");

        VkContext::SetDebugName(
            VK_OBJECT_TYPE_IMAGE,
            m_images[i],
            std::format("Image Swapchain {}", i));

        VkContext::SetDebugName(
            VK_OBJECT_TYPE_IMAGE_VIEW,
            m_imageViews[i],
            std::format("Image View Swapchain {}", i));
    }

    m_frameSyncData.reserve(m_framesCount);
    for (std::size_t i{ 0 }; i < m_framesCount; ++i)
    {
        m_frameSyncData.emplace_back(FrameSyncData{
            .renderFence = Fence(std::format("Fence_render_frame_{}", i), true),
            .imageAcquiredSemaphore = Semaphore(
                std::format("Semaphore_image_acquired_frame_{}", i),
                false),
            .renderCompleteSemaphore = Semaphore(
                std::format("Semaphore_render_complete_frame_{}", i),
                false),
        });
    }

    assert(m_framesCount < m_imageCount);
    LOG_DEBUG("Swapchain | Extent {}x{}", m_extent.width, m_extent.height);
    LOG_DEBUG("Swapchain | Frames count {}", m_framesCount);
    LOG_DEBUG("Swapchain | Images count {}", m_imageCount);
}

void Swapchain::Destroy()
{
    DestroyResources();

    vkDestroySwapchainKHR(
        VkContext::LogicalDevice(),
        m_handle,
        allocationCallbacks.get());
    m_handle = VK_NULL_HANDLE;
}

void Swapchain::Present(VkCommandBuffer commandBuffer)
{
    assert(GetLayout() == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    VkCommandBufferSubmitInfo submitInfo{ createVkCommandBufferSubmitInfo(
        commandBuffer) };

    VkSemaphoreSubmitInfo waitSemaphoreInfo{ createVkSemaphoreSubmitInfo(
        CurrentFrame().imageAcquiredSemaphore.GetHandle(),
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT) };

    VkSemaphoreSubmitInfo signalSemaphoreInfo{ createVkSemaphoreSubmitInfo(
        CurrentFrame().renderCompleteSemaphore.GetHandle(),
        VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT) };

    VkContext::Device()
        .GetQueue(QueueType::Graphics)
        .Submit(
            CurrentFrame().renderFence.GetHandle(),
            1,
            &waitSemaphoreInfo,
            1,
            &submitInfo,
            1,
            &signalSemaphoreInfo);

    VkSemaphore waitSemaphores[]{
        CurrentFrame().renderCompleteSemaphore.GetHandle()
    };

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = waitSemaphores;

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_handle;

    presentInfo.pImageIndices = &m_imageIndex;

    // A pointer to an array of at least swapchainCount element.
    // The result of the presenting operation will be stored in each of its
    // elements, for each swap chain respectively.
    // A single value returned by the whole function is the same as the worst
    // result value from all swapchains.
    // presentInfo.pResults = pResults;

    VkResult presentResult{
        vkQueuePresentKHR(VkContext::GetQueue(QueueType::Present), &presentInfo)
    };

    if (presentResult == VK_ERROR_OUT_OF_DATE_KHR ||
        presentResult == VK_SUBOPTIMAL_KHR)
    {
        m_resizeRequired = true;
        return;
    }
    else if (presentResult != VK_SUCCESS)
    {
        LOG_ERROR("Failed to present swapchain image");
        return;
    }
}

void Swapchain::AcquireNextImage()
{
    m_frameIndex = (m_frameIndex + 1) % m_framesCount;

    CurrentFrame().renderFence.Wait();

    VkResult result{ vkAcquireNextImageKHR(
        VkContext::LogicalDevice(),
        m_handle,
        UINT64_MAX,
        CurrentFrame().imageAcquiredSemaphore.GetHandle(),
        VK_NULL_HANDLE,
        &m_imageIndex) };

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        m_resizeRequired = true;
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        LOG_ERROR("Failed to acquire swapchain image");
        return;
    }

    CurrentFrame().renderFence.Reset();
}

void Swapchain::Resize(std::uint32_t width, std::uint32_t height)
{
    assert(width > 0 && height > 0);
    assert(
        width <= VkContext::Device().GetLimits().maxImageDimension2D &&
        height <= VkContext::Device().GetLimits().maxImageDimension2D);

    m_extent = VkExtent2D{ .width = width, .height = height };

    m_imageIndex = std::numeric_limits<std::uint32_t>::max();
    m_frameIndex = std::numeric_limits<std::uint32_t>::max();

    DestroyResources();

    VkSwapchainKHR oldSwapchain{ m_handle };
    Create();

    vkDestroySwapchainKHR(
        VkContext::LogicalDevice(),
        oldSwapchain,
        allocationCallbacks.get());
    oldSwapchain = VK_NULL_HANDLE;

    m_resizeRequired = false;
}

void Swapchain::BlitToSwapchain(
    const CommandBuffer& commandBuffer,
    const Image& image)
{
    assert(image.GetLayout() == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    VkImageBlit2 region{};
    region.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;

    region.srcSubresource.aspectMask = image.GetAspectMask();
    region.srcSubresource.mipLevel = 0;
    region.srcSubresource.baseArrayLayer = 0;
    region.srcSubresource.layerCount = 1;

    region.srcOffsets[1].x = static_cast<std::int32_t>(image.GetWidth());
    region.srcOffsets[1].y = static_cast<std::int32_t>(image.GetHeight());
    region.srcOffsets[1].z = static_cast<std::int32_t>(image.GetDepth());

    region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.dstSubresource.mipLevel = 0;
    region.dstSubresource.baseArrayLayer = 0;
    region.dstSubresource.layerCount = 1;

    region.dstOffsets[1].x = static_cast<std::int32_t>(GetWidth());
    region.dstOffsets[1].y = static_cast<std::int32_t>(GetHeight());
    region.dstOffsets[1].z = 1;

    SetLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkImageBlit2 regions[1]{ region };
    commandBuffer.BlitImage(image.GetHandle(), GetImage(), regions);
}

void Swapchain::SetVsync(const bool enable)
{
    // VK_PRESENT_MODE_IMMEDIATE_KHR
    if ((m_presentMode != VK_PRESENT_MODE_MAILBOX_KHR) != enable)
    {
        if (enable)
        {
            SurfaceDetails surfaceDetails{ getSurfaceDetails(
                VkContext::Device().GetPhysicalDevice(),
                m_surface) };

            m_presentMode = selectPresentMode(
                surfaceDetails.presentModes,
                VK_PRESENT_MODE_FIFO_KHR);
        }
        else
        {
            m_presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
        }

        m_resizeRequired = true;

        LOG_INFO("VSync has been {}", enable ? "enabled" : "disabled");
    }
}

bool Swapchain::IsVSync() const
{
    // VK_PRESENT_MODE_IMMEDIATE_KHR
    return m_presentMode == VK_PRESENT_MODE_FIFO_KHR;
}

const VkSwapchainKHR& Swapchain::GetHandle() const
{
    return m_handle;
}

const VkExtent2D& Swapchain::GetExtent() const
{
    return m_extent;
}

std::uint32_t Swapchain::GetWidth() const
{
    return m_extent.width;
}

std::uint32_t Swapchain::GetHeight() const
{
    return m_extent.height;
}

std::uint32_t Swapchain::GetFramesCount() const
{
    return m_framesCount;
}

std::uint32_t Swapchain::GetFrameIndex() const
{
    return m_frameIndex;
}

VkImage Swapchain::GetImage() const
{
    return m_images[m_imageIndex];
}

VkImageView Swapchain::GetImageView() const
{
    return m_imageViews[m_imageIndex];
}

std::uint32_t Swapchain::GetImageCount() const
{
    return m_imageCount;
}

std::uint32_t Swapchain::GetImageIndex() const
{
    return m_imageIndex;
}

const VkFormat& Swapchain::GetFormat() const
{
    return m_imageFormat;
}

VkImageLayout Swapchain::GetLayout() const
{
    return m_layouts[m_imageIndex];
}

void Swapchain::SetLayout(
    const CommandBuffer& commandBuffer,
    VkImageLayout layout)
{
    if (m_layouts[m_imageIndex] == layout)
        return;

    commandBuffer
        .TransitionImageLayout(GetImage(), GetFormat(), GetLayout(), layout);

    m_layouts[m_imageIndex] = layout;
}

bool Swapchain::IsResizeRequired() const
{
    return m_resizeRequired;
}

constexpr Swapchain::FrameSyncData& Swapchain::CurrentFrame()
{
    return m_frameSyncData[m_frameIndex];
}

void Swapchain::DestroyResources()
{
    for (std::size_t i{ 0 }; i < m_framesCount; ++i)
    {
        m_frameSyncData[i].renderFence.Destroy();
        m_frameSyncData[i].imageAcquiredSemaphore.Destroy();
        m_frameSyncData[i].renderCompleteSemaphore.Destroy();
    }

    m_frameSyncData.clear();

    for (auto& imageView : m_imageViews)
    {
        vkDestroyImageView(
            VkContext::LogicalDevice(),
            imageView,
            allocationCallbacks.get());
    }

    m_imageViews.clear();
    m_images.clear();
}

VkSurfaceFormatKHR Swapchain::selectSurfaceFormat(
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

VkPresentModeKHR Swapchain::selectPresentMode(
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

    LOG_WARNING("Requested present mode is not supported. Fallback to "
                "VK_PRESENT_MODE_FIFO_KHR");
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Swapchain::selectExtent(
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
