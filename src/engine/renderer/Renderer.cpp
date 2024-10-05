#include "Renderer.hpp"
#include "core/logger.hpp"
#include "vulkan/VulkanContext.hpp"
#include "vulkan/vulkan_command.hpp"
#include "vulkan/vulkan_debug.hpp"
#include "vulkan/vulkan_image.hpp"
#include "vulkan/vulkan_sync.hpp"
#include "window/Window.hpp"

namespace Zeus
{
Renderer::Renderer(Window& window, VulkanContext& vkContext)
    : m_window{ window },
      m_vkContext{ vkContext }
{
}

void Renderer::Init()
{
    m_frames.resize(m_vkContext.GetSwapchain().maxConcurrentFrames);

    InitSyncObjects();
    InitCommands();
    InitDrawObjects();
}

void Renderer::Destroy()
{
    debug("Destroying Renderer");
    destroyImage(
        m_vkContext.GetDevice().logicalDevice,
        m_vkContext.GetAllocator(),
        drawImage);

    destroyImage(
        m_vkContext.GetDevice().logicalDevice,
        m_vkContext.GetAllocator(),
        depthImage);

    for (std::size_t i{ 0 }; i < m_vkContext.GetSwapchain().maxConcurrentFrames;
         ++i)
    {
        vkDestroyCommandPool(
            m_vkContext.GetDevice().logicalDevice,
            m_frames[i].commandPool,
            allocationCallbacks.get());
    }

    for (std::size_t i{ 0 }; i < m_vkContext.GetSwapchain().maxConcurrentFrames;
         ++i)
    {
        vkDestroyFence(
            m_vkContext.GetDevice().logicalDevice,
            m_frames[i].renderFence,
            allocationCallbacks.get());

        vkDestroySemaphore(
            m_vkContext.GetDevice().logicalDevice,
            m_frames[i].imageAcquiredSemaphore,
            allocationCallbacks.get());

        vkDestroySemaphore(
            m_vkContext.GetDevice().logicalDevice,
            m_frames[i].renderCompleteSemaphore,
            allocationCallbacks.get());
    }
}

void Renderer::Draw()
{
}

void Renderer::BeginFrame()
{
    VKCHECK(
        vkWaitForFences(
            m_vkContext.GetDevice().logicalDevice,
            1,
            &getCurrentFrame().renderFence,
            VK_TRUE,
            UINT64_MAX),
        "Failed to wait for fence");

    // getCurrentFrame().deletionQueue.flush();
    // getCurrentFrame().descriptorAllocator.Clear(vkContext.device.logicalDevice);

    VkResult result{ vkAcquireNextImageKHR(
        m_vkContext.GetDevice().logicalDevice,
        m_vkContext.GetSwapchain().handle,
        UINT64_MAX,
        getCurrentFrame().imageAcquiredSemaphore,
        VK_NULL_HANDLE,
        &swapchainImageIndex) };

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        // resizeRequested = true;
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        error("Failed to acquire swapchain image");
        return;
    }

    drawExtent = {
        .width = static_cast<std::uint32_t>(
            static_cast<float>(drawImage.imageExtent.width) * m_renderScale),
        .height = static_cast<std::uint32_t>(
            static_cast<float>(drawImage.imageExtent.height) * m_renderScale),
    };

    VKCHECK(
        vkResetFences(
            m_vkContext.GetDevice().logicalDevice,
            1,
            &getCurrentFrame().renderFence),
        "Failed to reset fence");

    VKCHECK(
        vkResetCommandBuffer(getCurrentFrame().mainCommandBuffer, 0),
        "Failed to reset command buffer");

    beginVkCommandBuffer(
        getCurrentFrame().mainCommandBuffer,
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    transitionImageLayout(
        getCurrentFrame().mainCommandBuffer,
        drawImage.image,
        drawImage.imageFormat,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_GENERAL);

    transitionImageLayout(
        getCurrentFrame().mainCommandBuffer,
        depthImage.image,
        depthImage.imageFormat,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
}

void Renderer::EndFrame()
{
    VKCHECK(
        vkEndCommandBuffer(getCurrentFrame().mainCommandBuffer),
        "Failed to record command buffer");

    VkCommandBufferSubmitInfo submitInfo{ createVkCommandBufferSubmitInfo(
        getCurrentFrame().mainCommandBuffer) };

    VkSemaphoreSubmitInfo waitSemaphoreInfo{ createVkSemaphoreSubmitInfo(
        getCurrentFrame().imageAcquiredSemaphore,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT) };

    VkSemaphoreSubmitInfo signalSemaphoreInfo{ createVkSemaphoreSubmitInfo(
        getCurrentFrame().renderCompleteSemaphore,
        VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT) };

    cmdVkQueueSubmit2(
        m_vkContext.GetDevice().graphicsQueue,
        getCurrentFrame().renderFence,
        1,
        &waitSemaphoreInfo,
        1,
        &submitInfo,
        1,
        &signalSemaphoreInfo);

    VkResult presentResult{ cmdVkQueuePresentKHR(
        m_vkContext.GetDevice().presentQueue,
        1,
        &getCurrentFrame().renderCompleteSemaphore,
        1,
        &m_vkContext.GetSwapchain().handle,
        &swapchainImageIndex) };

    if (presentResult == VK_ERROR_OUT_OF_DATE_KHR ||
        presentResult == VK_SUBOPTIMAL_KHR || m_window.resized)
    {
        // resizeRequested = true;
    }
    else if (presentResult != VK_SUCCESS)
    {
        error("Failed to present swapchain image");
        return;
    }

    m_currentFrame =
        (m_currentFrame + 1) % m_vkContext.GetSwapchain().maxConcurrentFrames;
}

void Renderer::ResizeDrawObjects(const VkExtent2D& extent)
{
    destroyImage(
        m_vkContext.GetDevice().logicalDevice,
        m_vkContext.GetAllocator(),
        drawImage);

    destroyImage(
        m_vkContext.GetDevice().logicalDevice,
        m_vkContext.GetAllocator(),
        depthImage);

    InitDrawObjects();
}

void Renderer::InitSyncObjects()
{
    for (std::size_t i{ 0 }; i < m_vkContext.GetSwapchain().maxConcurrentFrames;
         ++i)
    {
        VKCHECK(
            createVkFence(
                m_vkContext.GetDevice().logicalDevice,
                true,
                m_frames[i].renderFence),
            "Failed to create frame fence.");

        VKCHECK(
            createVkSemaphore(
                m_vkContext.GetDevice().logicalDevice,
                m_frames[i].imageAcquiredSemaphore),
            "Failed to create frame semaphore.");

        VKCHECK(
            createVkSemaphore(
                m_vkContext.GetDevice().logicalDevice,
                m_frames[i].renderCompleteSemaphore),
            "Failed to create frame semaphore.");

#ifndef NDEBUG
        std::string renderFenceName{ "Fence Frame " + std::to_string(i) };
        setDebugUtilsObjectNameEXT(
            m_vkContext.GetDevice().logicalDevice,
            VK_OBJECT_TYPE_FENCE,
            reinterpret_cast<std::uint64_t>(m_frames[i].renderFence),
            renderFenceName.c_str());

        std::string imageSemaphoreName{ "Semaphore Frame Image " +
                                        std::to_string(i) };
        setDebugUtilsObjectNameEXT(
            m_vkContext.GetDevice().logicalDevice,
            VK_OBJECT_TYPE_SEMAPHORE,
            reinterpret_cast<std::uint64_t>(m_frames[i].imageAcquiredSemaphore),
            imageSemaphoreName.c_str());

        std::string renderSemaphoreName{ "Semaphore Frame Render " +
                                         std::to_string(i) };
        setDebugUtilsObjectNameEXT(
            m_vkContext.GetDevice().logicalDevice,
            VK_OBJECT_TYPE_SEMAPHORE,
            reinterpret_cast<std::uint64_t>(
                m_frames[i].renderCompleteSemaphore),
            renderSemaphoreName.c_str());
#endif
    }
}

void Renderer::InitCommands()
{
    for (std::size_t i{ 0 }; i < m_vkContext.GetSwapchain().maxConcurrentFrames;
         ++i)
    {
        createVkCommandPool(
            m_vkContext.GetDevice().logicalDevice,
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            m_vkContext.GetDevice().graphicsFamily,
            m_frames[i].commandPool);

        allocateVkCommandBuffer(
            m_frames[i].mainCommandBuffer,
            m_vkContext.GetDevice().logicalDevice,
            m_frames[i].commandPool);

#ifndef NDEBUG
        std::string commandPoolName{ "CommandPool Frame " + std::to_string(i) };
        setDebugUtilsObjectNameEXT(
            m_vkContext.GetDevice().logicalDevice,
            VK_OBJECT_TYPE_COMMAND_POOL,
            reinterpret_cast<std::uint64_t>(m_frames[i].commandPool),
            commandPoolName.c_str());

        std::string commandBufferName{ "CommandBuffer Frame " +
                                       std::to_string(i) };
        setDebugUtilsObjectNameEXT(
            m_vkContext.GetDevice().logicalDevice,
            VK_OBJECT_TYPE_COMMAND_BUFFER,
            reinterpret_cast<std::uint64_t>(m_frames[i].mainCommandBuffer),
            commandBufferName.c_str());
#endif
    }
}

void Renderer::InitDrawObjects()
{
    VkExtent3D drawImageExtent = {
        .width = m_window.extent.width,
        .height = m_window.extent.height,
        .depth = 1,
    };

    drawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
    drawImage.imageExtent = drawImageExtent;

    create2DImage(
        m_vkContext.GetAllocator(),
        drawImage,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | // todo remove
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT |
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    create2DImageView(
        m_vkContext.GetDevice().logicalDevice,
        drawImage,
        VK_IMAGE_ASPECT_COLOR_BIT);

    depthImage.imageFormat = VK_FORMAT_D32_SFLOAT;
    depthImage.imageExtent = drawImageExtent;

    create2DImage(
        m_vkContext.GetAllocator(),
        depthImage,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    create2DImageView(
        m_vkContext.GetDevice().logicalDevice,
        depthImage,
        VK_IMAGE_ASPECT_DEPTH_BIT);

#ifndef NDEBUG
    setDebugUtilsObjectNameEXT(
        m_vkContext.GetDevice().logicalDevice,
        VK_OBJECT_TYPE_IMAGE,
        reinterpret_cast<std::uint64_t>(drawImage.image),
        "Image Draw Color");

    setDebugUtilsObjectNameEXT(
        m_vkContext.GetDevice().logicalDevice,
        VK_OBJECT_TYPE_IMAGE,
        reinterpret_cast<std::uint64_t>(depthImage.image),
        "Image Draw Depth");

    setDebugUtilsObjectNameEXT(
        m_vkContext.GetDevice().logicalDevice,
        VK_OBJECT_TYPE_IMAGE_VIEW,
        reinterpret_cast<std::uint64_t>(drawImage.imageView),
        "Image View Draw Color");

    setDebugUtilsObjectNameEXT(
        m_vkContext.GetDevice().logicalDevice,
        VK_OBJECT_TYPE_IMAGE_VIEW,
        reinterpret_cast<std::uint64_t>(depthImage.imageView),
        "Image View Draw Depth");
#endif
}
}
