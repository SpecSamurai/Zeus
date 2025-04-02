#pragma once

#include "CommandBuffer.hpp"
#include "Fence.hpp"
#include "Semaphore.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <limits>
#include <vector>

namespace Zeus
{
class Swapchain
{
    struct FrameSyncData
    {
        Fence renderFence;

        Semaphore imageAcquiredSemaphore;
        Semaphore renderCompleteSemaphore;
    };

public:
    Swapchain() = default;
    Swapchain(
        const VkSurfaceKHR surface,
        const std::uint32_t width,
        const std::uint32_t height,
        const VkPresentModeKHR presentMode,
        const std::uint32_t frameCount);

    Swapchain(const Swapchain&) = delete;
    Swapchain& operator=(const Swapchain&) = delete;

    Swapchain(Swapchain&& other) noexcept;
    Swapchain& operator=(Swapchain&& other);

    ~Swapchain();

    void Create();
    void Destroy();

    void Present(VkCommandBuffer commandBuffer);
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

    const VkFormat& GetFormat() const;

    VkImageLayout GetLayout() const;
    void SetLayout(const CommandBuffer& commandBuffer, VkImageLayout layout);

    bool IsResizeRequired() const;

private:
    constexpr FrameSyncData& CurrentFrame();

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

private:
    VkSurfaceKHR m_surface{ VK_NULL_HANDLE };
    VkSwapchainKHR m_handle{ VK_NULL_HANDLE };

    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
    std::vector<VkImageLayout> m_layouts;

    std::vector<FrameSyncData> m_frameSyncData;

    std::uint32_t m_imageIndex{ std::numeric_limits<std::uint32_t>::max() };
    std::uint32_t m_imageCount;
    std::uint32_t m_frameIndex{ std::numeric_limits<std::uint32_t>::max() };
    std::uint32_t m_framesCount;

    VkExtent2D m_extent;

    VkPresentModeKHR m_presentMode;
    VkFormat m_imageFormat;

    bool m_resizeRequired{ false };
};
}
