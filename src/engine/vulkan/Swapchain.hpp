#pragma once

#include "events/WindowEvent.hpp"
#include "window/Window.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <limits>
#include <vector>

namespace Zeus
{
class Swapchain
{
    struct FrameData
    {
        VkSemaphore imageAcquiredSemaphore;
        VkSemaphore renderCompleteSemaphore;

        VkFence renderFence;
    };

public:
    Swapchain(
        const Window& window,
        VkSurfaceKHR surface,
        std::uint32_t width,
        std::uint32_t height,
        const VkPresentModeKHR presentMode,
        const std::uint32_t imageCount);

    void Create();
    void Destroy();

    void Present(VkCommandBuffer cmd);
    void AcquireNextImage();

    void Resize(std::uint32_t width, std::uint32_t height);

    void SetVsync(const bool enable);
    bool IsVSync() const;

    const VkSwapchainKHR& GetHandle() const;
    const VkExtent2D& GetExtent() const;
    std::uint32_t GetWidth() const;
    std::uint32_t GetHeight() const;
    std::uint32_t GetFramesCount() const;
    std::uint32_t GetFrameIndex() const;

    VkImage GetImage() const;
    VkImageView GetImageView() const;
    VkFormat GetFormat() const;
    std::uint32_t GetImageCount() const;
    std::uint32_t GetImageIndex() const;

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
    bool OnWindowResized(const WindowResizedEvent& event);

    VkSurfaceFormatKHR selectSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR>& surfaceFormats,
        const std::vector<VkSurfaceFormatKHR>& desiredFormats);

    VkPresentModeKHR selectPresentMode(
        const std::vector<VkPresentModeKHR>& presentModes,
        VkPresentModeKHR desiredPresentMode);

    VkExtent2D selectExtent(
        const VkSurfaceCapabilitiesKHR& surfaceCapabilities,
        const VkExtent2D& desiredExtent);

    inline constexpr FrameData& CurrentFrame()
    {
        return m_frames[m_frameIndex];
    }

private:
    const Window& m_window;
    VkSurfaceKHR m_surface{ VK_NULL_HANDLE };
    VkSwapchainKHR m_handle{ VK_NULL_HANDLE };
    VkSwapchainKHR m_oldHandle{ VK_NULL_HANDLE };

    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
    std::vector<FrameData> m_frames;

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

    std::uint32_t m_imageIndex{ std::numeric_limits<std::uint32_t>::max() };
    std::uint32_t m_imageCount;
    std::uint32_t m_frameIndex{ std::numeric_limits<std::uint32_t>::max() };
    std::uint32_t m_framesCount;

    VkExtent2D m_extent;

    VkPresentModeKHR m_presentMode;
    VkFormat m_imageFormat;

    bool m_windowed = false;
};
}
