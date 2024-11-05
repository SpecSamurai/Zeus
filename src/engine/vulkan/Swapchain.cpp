#include "Swapchain.hpp"

#include "VkContext.hpp"
#include "api/vulkan_command.hpp"
#include "api/vulkan_debug.hpp"
#include "api/vulkan_device.hpp"
#include "api/vulkan_sync.hpp"
#include "logging/logger.hpp"

#include <vulkan/vulkan_core.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
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
    assert(width > 0 && height > 0 && "Invalid resolution");
    assert(frameCount >= 2 && "frameCount cannot be less than 2");

    Create();
}

void Swapchain::Create()
{
    SurfaceDetails surfaceDetails{
        getSurfaceDetails(VkContext::GetDevice().GetPhysicalDevice(), m_surface)
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
        VkContext::GetDevice().GetQueueFamily(QueueType::Graphics),
    };

    std::uint32_t presentFamily{
        VkContext::GetDevice().GetQueueFamily(QueueType::Present),
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
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkSurfaceTransformFlagBitsKHR preTransform{
        VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
    };

    if (surfaceDetails.capabilities.supportedTransforms & preTransform)
        createInfo.preTransform = preTransform;
    else
        createInfo.preTransform = surfaceDetails.capabilities.currentTransform;

    VKCHECK(
        vkCreateSwapchainKHR(
            VkContext::GetLogicalDevice(),
            &createInfo,
            allocationCallbacks.get(),
            &m_handle),
        "Failed to create swap chain.");

    m_imageFormat = surfaceFormat.format;

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

    assert(m_images.size() == m_imageCount);

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

    m_frames.resize(m_framesCount);
    for (std::size_t i{ 0 }; i < m_framesCount; ++i)
    {
        VKCHECK(
            createVkFence(
                VkContext::GetLogicalDevice(),
                true,
                &m_frames[i].renderFence),
            "Failed to create frame fence.");

        VKCHECK(
            createVkSemaphore(
                VkContext::GetLogicalDevice(),
                &m_frames[i].imageAcquiredSemaphore),
            "Failed to create frame semaphore.");

        VKCHECK(
            createVkSemaphore(
                VkContext::GetLogicalDevice(),
                &m_frames[i].renderCompleteSemaphore),
            "Failed to create frame semaphore.");

#ifndef NDEBUG
        std::string renderFenceName{ "Fence Frame " + std::to_string(i) };
        setDebugUtilsObjectNameEXT(
            VkContext::GetLogicalDevice(),
            VK_OBJECT_TYPE_FENCE,
            reinterpret_cast<std::uint64_t>(m_frames[i].renderFence),
            renderFenceName.c_str());

        std::string imageSemaphoreName{ "Semaphore Frame Image " +
                                        std::to_string(i) };
        setDebugUtilsObjectNameEXT(
            VkContext::GetLogicalDevice(),
            VK_OBJECT_TYPE_SEMAPHORE,
            reinterpret_cast<std::uint64_t>(m_frames[i].imageAcquiredSemaphore),
            imageSemaphoreName.c_str());

        std::string renderSemaphoreName{ "Semaphore Frame Render " +
                                         std::to_string(i) };
        setDebugUtilsObjectNameEXT(
            VkContext::GetLogicalDevice(),
            VK_OBJECT_TYPE_SEMAPHORE,
            reinterpret_cast<std::uint64_t>(
                m_frames[i].renderCompleteSemaphore),
            renderSemaphoreName.c_str());
#endif
    }

    LOG_DEBUG("Swapchain: Frames count {}", m_framesCount);
    LOG_DEBUG("Swapchain: Images count {}", m_imageCount);
}

void Swapchain::Destroy()
{
    DestroyResources();

    vkDestroySwapchainKHR(
        VkContext::GetLogicalDevice(),
        m_handle,
        allocationCallbacks.get());

    m_handle = VK_NULL_HANDLE;
}

void Swapchain::Present(VkCommandBuffer cmd)
{
    // SP_ASSERT(m_layouts[m_image_index] ==
    // RHI_Image_Layout::Present_Source);
    //
    // m_wait_semaphores.clear();
    // RHI_Queue* queue = RHI_Device::GetQueue(RHI_Queue_Type::Graphics);
    //
    // // semaphores from command lists
    // RHI_CommandList* cmd_list = queue->GetCommandList();
    // bool presents_to_this_swapchain =
    //     cmd_list->GetSwapchainId() == m_object_id;
    // bool has_work_to_present =
    //     cmd_list->GetState() == RHI_CommandListState::Submitted;
    // if (presents_to_this_swapchain && has_work_to_present)
    // {
    //     RHI_Semaphore* semaphore =
    //         cmd_list->GetRenderingCompleteSemaphore();
    //     if (semaphore->IsSignaled())
    //     {
    //         semaphore->SetSignaled(false);
    //     }
    //
    //     m_wait_semaphores.emplace_back(semaphore);
    // }
    //
    // // semaphore from vkAcquireNextImageKHR
    // RHI_Semaphore* image_acquired_semaphore =
    //     m_image_acquired_semaphore[m_sync_index].get();
    // m_wait_semaphores.emplace_back(image_acquired_semaphore);
    //
    // // present
    // queue->Present(m_rhi_swapchain, m_image_index, m_wait_semaphores);
    // AcquireNextImage();

    VkCommandBufferSubmitInfo submitInfo{ createVkCommandBufferSubmitInfo(
        cmd) };

    VkSemaphoreSubmitInfo waitSemaphoreInfo{ createVkSemaphoreSubmitInfo(
        CurrentFrame().imageAcquiredSemaphore,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT) };

    VkSemaphoreSubmitInfo signalSemaphoreInfo{ createVkSemaphoreSubmitInfo(
        CurrentFrame().renderCompleteSemaphore,
        VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT) };

    cmdVkQueueSubmit2(
        VkContext::GetDevice().GetQueue(QueueType::Graphics),
        CurrentFrame().renderFence,
        1,
        &waitSemaphoreInfo,
        1,
        &submitInfo,
        1,
        &signalSemaphoreInfo);

    VkResult presentResult{ cmdVkQueuePresentKHR(
        VkContext::GetDevice().GetQueue(QueueType::Present),
        1,
        &CurrentFrame().renderCompleteSemaphore,
        1,
        &m_handle,
        &m_imageIndex) };

    if (presentResult == VK_ERROR_OUT_OF_DATE_KHR ||
        presentResult == VK_SUBOPTIMAL_KHR)
    {
        // m_swapchainRebuildRequired = true;
    }
    else if (presentResult != VK_SUCCESS)
    {
        LOG_ERROR("Failed to present swapchain image");
    }

    // m_currentFrame = (m_currentFrame + 1) %
    // m_swapchain->GetFramesCount();

    // VkCommandBufferSubmitInfo submitInfo{ createVkCommandBufferSubmitInfo(
    //     CurrentFrame().mainCommandBuffer) };

    // VkSemaphoreSubmitInfo waitSemaphoreInfo{ createVkSemaphoreSubmitInfo(
    //     CurrentFrame().imageAcquiredSemaphore,
    //     VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT) };
    //
    // VkSemaphoreSubmitInfo signalSemaphoreInfo{ createVkSemaphoreSubmitInfo(
    //     CurrentFrame().renderCompleteSemaphore,
    //     VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT) };
    //
    // cmdVkQueueSubmit2(
    //     VkContext::GetDevice().GetQueue(QueueType::Graphics),
    //     CurrentFrame().renderFence,
    //     1,
    //     &waitSemaphoreInfo,
    //     1,
    //     &submitInfo,
    //     1,
    //     &signalSemaphoreInfo);
    //
    // VkResult presentResult{ cmdVkQueuePresentKHR(
    //     VkContext::GetDevice().GetQueue(QueueType::Present),
    //     1,
    //     &CurrentFrame().renderCompleteSemaphore,
    //     1,
    //     &m_swapchain->GetHandle(),
    //     &m_swapchainImageIndex) };
    //
    // if (presentResult == VK_ERROR_OUT_OF_DATE_KHR ||
    //     presentResult == VK_SUBOPTIMAL_KHR)
    // {
    //     m_swapchainRebuildRequired = true;
    // }
    // else if (presentResult != VK_SUCCESS)
    // {
    //     LOG_ERROR("Failed to present swapchain image");
    //     return presentResult;
    // }
    //
    // m_currentFrame = (m_currentFrame + 1) % m_swapchain->GetFramesCount();
}

void Swapchain::AcquireNextImage()
{
    // if (m_sync_index != std::numeric_limits<uint32_t>::max())
    // {
    //     m_image_acquired_fence[m_sync_index]->Wait();
    //     m_image_acquired_fence[m_sync_index]->Reset();
    // }
    //
    // // get sync objects
    // m_sync_index = (m_sync_index + 1) % m_buffer_count;
    // RHI_Semaphore* signal_semaphore =
    //     m_image_acquired_semaphore[m_sync_index].get();
    // RHI_Fence* signal_fence = m_image_acquired_fence[m_sync_index].get();
    //
    // // acquire next image
    // SP_ASSERT_VK_MSG(
    //     vkAcquireNextImageKHR(
    //         RHI_Context::device,                          // device
    //         static_cast<VkSwapchainKHR>(m_rhi_swapchain), // swapchain
    //         numeric_limits<uint64_t>::max(), // timeout - wait/block
    //         static_cast<VkSemaphore>(
    //             signal_semaphore->GetRhiResource()), // signal semaphore
    //         static_cast<VkFence>(
    //             signal_fence->GetRhiResource()), // signal fence
    //         &m_image_index                       // pImageIndex
    //         ),
    //     "Failed to acquire next image");

    m_frameIndex = (m_frameIndex + 1) % m_framesCount;

    VKCHECK(
        vkWaitForFences(
            VkContext::GetLogicalDevice(),
            1,
            &CurrentFrame().renderFence,
            VK_TRUE,
            UINT64_MAX),
        "Failed to wait for fence");

    // getCurrentFrame().deletionQueue.flush();
    // getCurrentFrame().descriptorAllocator.Clear(vkContext.device.logicalDevice);

    VkResult result{ vkAcquireNextImageKHR(
        VkContext::GetLogicalDevice(),
        m_handle,
        UINT64_MAX,
        CurrentFrame().imageAcquiredSemaphore,
        VK_NULL_HANDLE,
        &m_imageIndex) };

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        // m_swapchainRebuildRequired = true;
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        LOG_ERROR("Failed to acquire swapchain image");
        return;
    }

    VKCHECK(
        vkResetFences(
            VkContext::GetLogicalDevice(),
            1,
            &CurrentFrame().renderFence),
        "Failed to reset fence");
}

void Swapchain::Resize(std::uint32_t width, std::uint32_t height)
{
    // SP_ASSERT(RHI_Device::IsValidResolution(width, height));
    // assert(width > 0 && height > 0 && "Invalid Swapchain size");
    // return width > 4 && width <= m_max_texture_2d_dimension && height > 4 &&
    //        height <= m_max_texture_2d_dimension;

    m_extent = VkExtent2D{ .width = width, .height = height };

    m_imageIndex = std::numeric_limits<std::uint32_t>::max();
    m_frameIndex = std::numeric_limits<std::uint32_t>::max();

    DestroyResources();

    VkSwapchainKHR oldSwapchain{ m_handle };
    Create();

    vkDestroySwapchainKHR(
        VkContext::GetLogicalDevice(),
        oldSwapchain,
        allocationCallbacks.get());
    oldSwapchain = VK_NULL_HANDLE;

    LOG_DEBUG("Swapchain resized: {}x{}", m_extent.width, m_extent.height);
}

void Swapchain::SetVsync(const bool enable)
{
    if ((m_presentMode != VK_PRESENT_MODE_IMMEDIATE_KHR) != enable)
    {
        if (enable)
        {
            SurfaceDetails surfaceDetails{ getSurfaceDetails(
                VkContext::GetDevice().GetPhysicalDevice(),
                m_surface) };

            m_presentMode = selectPresentMode(
                surfaceDetails.presentModes,
                VK_PRESENT_MODE_MAILBOX_KHR);
        }
        else
        {
            m_presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }

        Resize(m_extent.width, m_extent.height);

        LOG_INFO("VSync has been {}", enable ? "enabled" : "disabled");
    }
}

bool Swapchain::IsVSync() const
{
    return m_presentMode != VK_PRESENT_MODE_IMMEDIATE_KHR;
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

VkFormat Swapchain::GetFormat() const
{
    return m_imageFormat;
}

void Swapchain::DestroyResources()
{
    for (std::size_t i{ 0 }; i < m_framesCount; ++i)
    {
        vkDestroyFence(
            VkContext::GetLogicalDevice(),
            m_frames[i].renderFence,
            allocationCallbacks.get());

        vkDestroySemaphore(
            VkContext::GetLogicalDevice(),
            m_frames[i].imageAcquiredSemaphore,
            allocationCallbacks.get());

        vkDestroySemaphore(
            VkContext::GetLogicalDevice(),
            m_frames[i].renderCompleteSemaphore,
            allocationCallbacks.get());
    }

    m_frames.clear();

    for (auto& imageView : m_imageViews)
    {
        vkDestroyImageView(
            VkContext::GetLogicalDevice(),
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

constexpr Swapchain::FrameData& Swapchain::CurrentFrame()
{
    return m_frames[m_frameIndex];
}
}
