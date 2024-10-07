#pragma once

#include "vulkan/DynamicDescriptorAllocator.hpp"
#include "vulkan/VulkanContext.hpp"
#include "vulkan/vulkan_image.hpp"
#include "window/Window.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>

namespace Zeus
{
struct FrameData
{
    VkCommandPool commandPool;
    VkCommandBuffer mainCommandBuffer;

    VkSemaphore imageAcquiredSemaphore;
    VkSemaphore renderCompleteSemaphore;

    VkFence renderFence;

    DynamicDescriptorAllocator descriptorAllocator;

    // TODO: conisider adding draw resources per frame
    // Image Backbuffer;
};

class Renderer
{
public:
    Renderer(Window& window, VulkanContext& vkContext);

    void Init();
    void Destroy();

    VkResult BeginFrame();
    VkResult EndFrame();

    void Draw();

    void ResizeDrawObjects(const VkExtent2D& extent);

    inline constexpr bool ResizeRequired() const
    {
        return m_window.resized || m_swapchainRebuildRequired;
    }

    inline constexpr FrameData& CurrentFrame()
    {
        return m_frames[m_currentFrame];
    }

    inline constexpr std::uint32_t CurrentSwapchainImageIndex() const
    {
        return m_swapchainImageIndex;
    }

private:
    void InitSyncObjects();
    void InitCommands();
    void InitDrawObjects(const VkExtent2D& extent);

    Window& m_window;
    VulkanContext& m_vkContext;

    std::vector<FrameData> m_frames;

    std::uint32_t m_currentFrame{ 0 };
    std::uint32_t m_swapchainImageIndex;

    float m_renderScale{ 1.f };

    bool m_swapchainRebuildRequired{ false };

public:
    VkExtent2D drawExtent;
    Image drawImage;
    Image depthImage;
};
}
