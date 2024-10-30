#pragma once

#include "CommandBuffer.hpp"

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
        const VkSurfaceKHR surface,
        const std::uint32_t width,
        const std::uint32_t height,
        const VkPresentModeKHR presentMode,
        const std::uint32_t frameCount);

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
    std::uint32_t GetImageCount() const;
    std::uint32_t GetImageIndex() const;

    VkFormat GetFormat() const;

    VkImageLayout GetLayout() const;
    void SetLayout(VkImageLayout layout, CommandBuffer& commandBuffer)
    {
        // if (m_layouts[m_image_index] == layout)
        //     return;

        commandBuffer.InsertImageLayoutBarrier(layout);
        // commandBuffer.InsertBarrierTexture(
        //     m_rhi_rt[m_image_index],
        //     VK_IMAGE_ASPECT_COLOR_BIT,
        //     0,
        //     1,
        //     1,
        //     m_layouts[m_image_index],
        //     layout,
        //     false);
        //
        // m_layouts[m_image_index] = layout;
    }

private:
    void DestroyResources();

    VkSurfaceFormatKHR selectSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR>& surfaceFormats,
        const std::vector<VkSurfaceFormatKHR>& desiredFormats);

    VkPresentModeKHR selectPresentMode(
        const std::vector<VkPresentModeKHR>& presentModes,
        VkPresentModeKHR desiredPresentMode);

    VkExtent2D selectExtent(
        const VkSurfaceCapabilitiesKHR& surfaceCapabilities,
        const VkExtent2D& desiredExtent);

    constexpr FrameData& CurrentFrame();

private:
    VkSurfaceKHR m_surface{ VK_NULL_HANDLE };
    VkSwapchainKHR m_handle{ VK_NULL_HANDLE };

    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;

    std::vector<FrameData> m_frames;

    // std::array<VkImageLayout, 3> m_layouts = {
    //     VK_IMAGE_LAYOUT_UNDEFINED,
    //     VK_IMAGE_LAYOUT_UNDEFINED,
    //     VK_IMAGE_LAYOUT_UNDEFINED,
    // };
    // std::array<std::shared_ptr<Semaphore>, max_buffer_count>
    //     m_image_acquired_semaphore;
    // std::array<std::shared_ptr<Fence>, max_buffer_count>
    //     m_image_acquired_fence;
    // std::vector<Semaphore*> m_wait_semaphores;

    std::uint32_t m_imageIndex{ std::numeric_limits<std::uint32_t>::max() };
    std::uint32_t m_imageCount;
    std::uint32_t m_frameIndex{ std::numeric_limits<std::uint32_t>::max() };
    std::uint32_t m_framesCount;

    VkExtent2D m_extent;

    VkPresentModeKHR m_presentMode;
    VkFormat m_imageFormat;
};
}
