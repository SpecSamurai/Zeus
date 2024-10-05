#pragma once

#include "vulkan/DynamicDescriptorAllocator.hpp"
#include "vulkan/VulkanContext.hpp"
#include "vulkan/vulkan_image.hpp"
#include "window/Window.hpp"

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

    void Draw();
    void BeginFrame();
    void EndFrame();

    void ResizeDrawObjects(const VkExtent2D& extent);

    inline constexpr FrameData& getCurrentFrame()
    {
        return m_frames[m_currentFrame];
    }

private:
    void InitSyncObjects();
    void InitCommands();
    void InitDrawObjects();

    Window& m_window;
    VulkanContext& m_vkContext;

    std::vector<FrameData> m_frames;

    std::uint32_t m_currentFrame{ 0 };
    float m_renderScale{ 1.f };

public:
    std::uint32_t swapchainImageIndex;

    VkExtent2D drawExtent;
    Image drawImage;
    Image depthImage;
};
}
