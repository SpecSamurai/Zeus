#pragma once

#include "logging/logger.hpp"
#include "vulkan/VkContext.hpp"
#include "vulkan/api/vulkan_debug.hpp"
#include "vulkan/api/vulkan_device.hpp"
#include "window/Window.hpp"

#include <algorithm>
#include <optional>
#include <vulkan/vulkan_core.h>

#include <array>
#include <cstdint>
#include <vector>

namespace Zeus
{
class Swapchain2
{
public:
    Swapchain2() = default;
    Swapchain2(
        Window& window,
        const std::uint32_t width,
        const std::uint32_t height,
        // const VkPresentModeKHR presentMode,
        // const std::uint32_t buffer_count,
        // const bool hdr,
        const char* name)
        : m_window{ window }
    {
        // SP_ASSERT_MSG(RHI_Device::IsValidResolution(width, height), "Invalid
        // resolution"); SP_ASSERT_MSG(buffer_count >= 2, "Buffer count can't be
        // less than 2");

        // m_format       = hdr ? format_hdr : format_sdr;
        // m_buffer_count = buffer_count;
        m_width = width;
        m_height = height;
        m_extent = VkExtent2D{ .width = width, .height = height };
        // m_sdl_window   = sdl_window;
        // m_object_name  = name;
        // m_present_mode = present_mode;
        m_surface = VkContext::GetSurface();

        Create();
        AcquireNextImage();

        // SP_SUBSCRIBE_TO_EVENT(EventType::WindowResized,
        // SP_EVENT_HANDLER(ResizeToWindowSize));
    }

    ~Swapchain2()
    {
        // Destroy();
    }

    void Resize(
        std::uint32_t width,
        std::uint32_t height,
        const bool force = false)
    {
        // SP_ASSERT(RHI_Device::IsValidResolution(width, height));
        //
        // // only resize if needed
        // if (!force)
        // {
        //     if (m_width == width && m_height == height)
        //         return;
        // }
        //
        // // save new dimensions
        // m_width = width;
        // m_height = height;
        //
        // // reset indices
        // m_image_index = numeric_limits<uint32_t>::max();
        // m_sync_index = numeric_limits<uint32_t>::max();
        //
        // Destroy();
        // Create();
        // AcquireNextImage();
        //
        // SP_LOG_INFO("Resolution has been set to %dx%d", width, height);
    }

    void ResizeToWindowSize();

    VkFormat GetFormat() const;

    void Present()
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
    }

    // void SetHdr(const bool enabled);
    // bool IsHdr() const { return m_format == format_hdr; }

    void SetVsync(const bool enabled)
    {
        // For v-sync, we could Mailbox for lower latency, but Fifo is always
        // supported, so we'll assume that

        // if ((m_present_mode == RHI_Present_Mode::Fifo) != enabled)
        // {
        //     m_present_mode =
        //         enabled ? RHI_Present_Mode::Fifo :
        //         RHI_Present_Mode::Immediate;
        //     Resize(m_width, m_height, true);
        //     Timer::OnVsyncToggled(enabled);
        //     SP_LOG_INFO("VSync has been %s", enabled ? "enabled" :
        //     "disabled");
        // }
    }
    bool GetVsync();

    uint32_t GetWidth() const
    {
        return m_width;
    }

    uint32_t GetHeight() const
    {
        return m_height;
    }

    uint32_t GetBufferCount() const
    {
        return m_buffer_count;
    }

    void* GetRhiRt() const
    {
        // return m_rhi_rt[m_image_index];
    }

    void* GetRhiRtv() const
    {
        // return m_rhi_rtv[m_image_index];
    }

    // RHI_Image_Layout GetLayout() const;
    // void SetLayout(const RHI_Image_Layout& layout, RHI_CommandList* cmd_list)
    // {
    //     if (m_layouts[m_image_index] == layout)
    //         return;
    //
    //     cmd_list->InsertBarrierTexture(
    //         m_rhi_rt[m_image_index],
    //         VK_IMAGE_ASPECT_COLOR_BIT,
    //         0,
    //         1,
    //         1,
    //         m_layouts[m_image_index],
    //         layout,
    //         false);
    //
    //     m_layouts[m_image_index] = layout;
    // }

private:
    void Create();

    VkSurfaceFormatKHR selectSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR>& surfaceFormats,
        const std::vector<VkSurfaceFormatKHR>& desiredFormats);

    VkPresentModeKHR selectPresentMode(
        const std::vector<VkPresentModeKHR>& presentModes,
        VkPresentModeKHR desiredPresentMode);

    VkExtent2D selectExtent(
        const VkSurfaceCapabilitiesKHR& surfaceCapabilities,
        const VkExtent2D& desiredExtent);

    void Destroy()
    {
        // VkContext::GetDevice().QueueWaitAll();

        for (auto& imageView : m_imageViews)
        {
            vkDestroyImageView(
                VkContext::GetLogicalDevice(),
                imageView,
                allocationCallbacks.get());
        }

        // m_rhi_rtv.fill(nullptr);
        // m_image_acquired_semaphore.fill(nullptr);

        vkDestroySwapchainKHR(
            VkContext::GetLogicalDevice(),
            m_handle,
            allocationCallbacks.get());

        m_handle = VK_NULL_HANDLE;

        vkDestroySurfaceKHR(
            VkContext::GetInstance(),
            VkContext::GetSurface(),
            nullptr);

        m_surface = VK_NULL_HANDLE;
    }

    void AcquireNextImage()
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
    }

public:
    VkSwapchainKHR m_handle{ VK_NULL_HANDLE };
    VkSurfaceKHR m_surface{ VK_NULL_HANDLE };
    const Window& m_window;

    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;

    VkExtent2D m_extent;
    VkPresentModeKHR m_presentMode;

    std::uint32_t m_imageIndex;
    std::uint32_t m_imageCount;
    std::uint32_t m_maxConcurrentFrames;

    uint32_t m_buffer_count = 0;
    uint32_t m_width = 0;
    uint32_t m_height = 0;

    uint32_t m_sync_index = std::numeric_limits<uint32_t>::max();
    uint32_t m_image_index = std::numeric_limits<uint32_t>::max();
    // std::array<RHI_Image_Layout, max_buffer_count> m_layouts = {
    //     RHI_Image_Layout::Max,
    //     RHI_Image_Layout::Max,
    //     RHI_Image_Layout::Max
    // };
    // std::array<std::shared_ptr<RHI_Semaphore>, max_buffer_count>
    //     m_image_acquired_semaphore;
    // std::array<std::shared_ptr<RHI_Fence>, max_buffer_count>
    //     m_image_acquired_fence;
    // std::vector<RHI_Semaphore*> m_wait_semaphores;

    VkFormat m_imageFormat;
    VkImageUsageFlags m_imageUsageFlags;

    bool m_windowed = false;
};
}
