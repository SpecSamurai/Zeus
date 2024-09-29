#include "Engine.hpp"

#include "core/logger.hpp"
#include "vulkan/vulkan_command.hpp"
#include "vulkan/vulkan_debug.hpp"
#include "vulkan/vulkan_image.hpp"
#include "vulkan/vulkan_memory.hpp"
#include "vulkan/vulkan_sync.hpp"
#include "vulkan/vulkan_types.hpp"

#include <vulkan/vulkan_core.h>

#include <chrono>
#include <cstdint>
#include <string>
#include <thread>

namespace Zeus
{
Engine::Engine(std::uint32_t width, std::uint32_t height, const char* title)
    : vkContext(width, height, title)
{
}

void Engine::Init()
{
    vkContext.Init();

    frames.resize(vkContext.swapchain.maxConcurrentFrames);

    InitSyncObjects();
    InitCommands();
    InitDrawObjects();
}

void Engine::Run()
{
    while (!glfwWindowShouldClose(vkContext.window.handle))
    {
        glfwPollEvents();

        if (vkContext.window.invalidExtent) // freeze_rendering)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        if (vkContext.window.resized)
        {
            RecreateSwapchain();
        }


        Draw();
    }
}

void Engine::Shutdown()
{
    debug("Shutting down engine");
    vkDeviceWaitIdle(vkContext.device.logicalDevice);

    debug("Destroying draw objects");
    destroyImage(
        vkContext.device.logicalDevice,
        vkContext.allocator,
        drawImage);

    destroyImage(
        vkContext.device.logicalDevice,
        vkContext.allocator,
        depthImage);

    debug("Destroying commands");
    vkDestroyCommandPool(
        vkContext.device.logicalDevice,
        oneTimeSubmitCommandPool,
        allocationCallbacks.get());

    for (std::size_t i{ 0 }; i < vkContext.swapchain.maxConcurrentFrames; ++i)
    {
        vkDestroyCommandPool(
            vkContext.device.logicalDevice,
            frames[i].commandPool,
            allocationCallbacks.get());
    }

    debug("Destroying sync objects");
    vkDestroyFence(
        vkContext.device.logicalDevice,
        oneTimeSubmitFence,
        allocationCallbacks.get());

    for (std::size_t i{ 0 }; i < vkContext.swapchain.maxConcurrentFrames; ++i)
    {
        vkDestroyFence(
            vkContext.device.logicalDevice,
            frames[i].renderFence,
            allocationCallbacks.get());

        vkDestroySemaphore(
            vkContext.device.logicalDevice,
            frames[i].imageAcquiredSemaphore,
            allocationCallbacks.get());

        vkDestroySemaphore(
            vkContext.device.logicalDevice,
            frames[i].renderCompleteSemaphore,
            allocationCallbacks.get());
    }

    vkContext.Destroy();
}

void Engine::cmdOneTimeSubmit(
    std::function<void(VkCommandBuffer cmd)>&& function)
{
    VKCHECK(
        vkResetFences(vkContext.device.logicalDevice, 1, &oneTimeSubmitFence),
        "Failed to reset fence");

    VKCHECK(
        vkResetCommandBuffer(oneTimeSubmitCommandBuffer, 0),
        "Failed to reset command buffer");

    beginVkCommandBuffer(
        oneTimeSubmitCommandBuffer,
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    function(oneTimeSubmitCommandBuffer);

    VKCHECK(
        vkEndCommandBuffer(oneTimeSubmitCommandBuffer),
        "Failed to end command buffer");

    VkCommandBufferSubmitInfo submitInfo{ createVkCommandBufferSubmitInfo(
        oneTimeSubmitCommandBuffer) };

    cmdVkQueueSubmit2(
        vkContext.device.transferQueue,
        oneTimeSubmitFence,
        0,
        nullptr,
        1,
        &submitInfo,
        0,
        nullptr);

    VKCHECK(
        vkWaitForFences(
            vkContext.device.logicalDevice,
            1,
            &oneTimeSubmitFence,
            VK_TRUE,
            UINT64_MAX),
        "Failed to wait for fence");
}

void Engine::Draw()
{
    VKCHECK(
        vkWaitForFences(
            vkContext.device.logicalDevice,
            1,
            &getCurrentFrame().renderFence,
            VK_TRUE,
            UINT64_MAX),
        "Failed to wait for fence");

    // getCurrentFrame().deletionQueue.flush();
    getCurrentFrame().descriptorAllocator.Clear(vkContext.device.logicalDevice);

    std::uint32_t swapchainImageIndex;
    VkResult result{ vkAcquireNextImageKHR(
        vkContext.device.logicalDevice,
        vkContext.swapchain.handle,
        UINT64_MAX,
        getCurrentFrame().imageAcquiredSemaphore,
        VK_NULL_HANDLE,
        &swapchainImageIndex) };

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        // recreateSwapChain();
        resizeRequested = true;
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        error("Failed to acquire swapchain image");
        return;
    }

    drawExtent.height = std::min(
                            vkContext.swapchain.extent.height,
                            drawImage.imageExtent.height) *
                        1.f;
    drawExtent.width = std::min(
                           vkContext.swapchain.extent.width,
                           drawImage.imageExtent.width) *
                       1.f;

    VKCHECK(
        vkResetFences(
            vkContext.device.logicalDevice,
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

    // drawMain(getCurrentFrame().mainCommandBuffer);

    VkClearColorValue clearValue;
    clearValue = { { 0.0f, 0.0f, 1.f, 1.0f } };

    VkImageSubresourceRange subImage{};
    subImage.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subImage.baseMipLevel = 0;
    subImage.levelCount = VK_REMAINING_MIP_LEVELS;
    subImage.baseArrayLayer = 0;
    subImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

    vkCmdClearColorImage(
        getCurrentFrame().mainCommandBuffer,
        drawImage.image,
        VK_IMAGE_LAYOUT_GENERAL,
        &clearValue,
        1,
        &subImage);

    transitionImageLayout(
        getCurrentFrame().mainCommandBuffer,
        drawImage.image,
        drawImage.imageFormat,
        VK_IMAGE_LAYOUT_GENERAL,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    transitionImageLayout(
        getCurrentFrame().mainCommandBuffer,
        vkContext.swapchain.images[swapchainImageIndex],
        vkContext.swapchain.imageFormat,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // VkExtent2D extent;
    // extent.height = _windowExtent.height;
    // extent.width = _windowExtent.width;
    // extent.depth = 1;

    blitImage(
        getCurrentFrame().mainCommandBuffer,
        drawImage.image,
        vkContext.swapchain.images[swapchainImageIndex],
        drawExtent,
        vkContext.swapchain.extent);

    transitionImageLayout(
        getCurrentFrame().mainCommandBuffer,
        vkContext.swapchain.images[swapchainImageIndex],
        vkContext.swapchain.imageFormat,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    // drawUI(
    //     getCurrentFrame().mainCommandBuffer,
    //     vkContext.swapchain.imageViews[swapchainImageIndex]);

    transitionImageLayout(
        getCurrentFrame().mainCommandBuffer,
        vkContext.swapchain.images[swapchainImageIndex],
        vkContext.swapchain.imageFormat,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

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
        vkContext.device.graphicsQueue,
        getCurrentFrame().renderFence,
        1,
        &waitSemaphoreInfo,
        1,
        &submitInfo,
        1,
        &signalSemaphoreInfo);

    VkResult presentResult{ cmdVkQueuePresentKHR(
        vkContext.device.presentQueue,
        1,
        &getCurrentFrame().renderCompleteSemaphore,
        1,
        &vkContext.swapchain.handle,
        &swapchainImageIndex) };

    if (presentResult == VK_ERROR_OUT_OF_DATE_KHR ||
        presentResult == VK_SUBOPTIMAL_KHR || vkContext.window.resized)
    {
        resizeRequested = true;
        // recreateSwapChain();
    }
    else if (presentResult != VK_SUCCESS)
    {
        error("Failed to present swapchain image");
        return;
    }

    currentFrame = (currentFrame + 1) % vkContext.swapchain.maxConcurrentFrames;
}

void Engine::RecreateSwapchain()
{
    // Handles minimazation
    // int width = 0, height = 0;
    // glfwGetFramebufferSize(window, &width, &height);
    // while (width == 0 || height == 0)
    // {
    //     glfwGetFramebufferSize(window, &width, &height);
    //     glfwWaitEvents();
    // }

    vkContext.ResizeSwapchain(vkContext.window.extent);

    destroyImage(
        vkContext.device.logicalDevice,
        vkContext.allocator,
        drawImage);

    destroyImage(
        vkContext.device.logicalDevice,
        vkContext.allocator,
        depthImage);

    InitDrawObjects();

    vkContext.window.resized = false;
}

void Engine::InitSyncObjects()
{
    VKCHECK(
        createVkFence(vkContext.device.logicalDevice, true, oneTimeSubmitFence),
        "Failed to create oneTimeSubmit fence.");

    for (std::size_t i{ 0 }; i < vkContext.swapchain.maxConcurrentFrames; ++i)
    {
        VKCHECK(
            createVkFence(
                vkContext.device.logicalDevice,
                true,
                frames[i].renderFence),
            "Failed to create frame fence.");

        VKCHECK(
            createVkSemaphore(
                vkContext.device.logicalDevice,
                frames[i].imageAcquiredSemaphore),
            "Failed to create frame semaphore.");

        VKCHECK(
            createVkSemaphore(
                vkContext.device.logicalDevice,
                frames[i].renderCompleteSemaphore),
            "Failed to create frame semaphore.");

#ifndef NDEBUG
        std::string renderFenceName{ "Fence Frame " + std::to_string(i) };
        setDebugUtilsObjectNameEXT(
            vkContext.device.logicalDevice,
            VK_OBJECT_TYPE_FENCE,
            reinterpret_cast<std::uint64_t>(frames[i].renderFence),
            renderFenceName.c_str());

        std::string imageSemaphoreName{ "Semaphore Frame Image " +
                                        std::to_string(i) };
        setDebugUtilsObjectNameEXT(
            vkContext.device.logicalDevice,
            VK_OBJECT_TYPE_SEMAPHORE,
            reinterpret_cast<std::uint64_t>(frames[i].imageAcquiredSemaphore),
            imageSemaphoreName.c_str());

        std::string renderSemaphoreName{ "Semaphore Frame Render " +
                                         std::to_string(i) };
        setDebugUtilsObjectNameEXT(
            vkContext.device.logicalDevice,
            VK_OBJECT_TYPE_SEMAPHORE,
            reinterpret_cast<std::uint64_t>(frames[i].renderCompleteSemaphore),
            renderSemaphoreName.c_str());
#endif
    }

#ifndef NDEBUG
    setDebugUtilsObjectNameEXT(
        vkContext.device.logicalDevice,
        VK_OBJECT_TYPE_FENCE,
        reinterpret_cast<std::uint64_t>(oneTimeSubmitFence),
        "Fence One-Time Submit");
#endif
}

void Engine::InitCommands()
{
    for (std::size_t i{ 0 }; i < vkContext.swapchain.maxConcurrentFrames; ++i)
    {
        createVkCommandPool(
            vkContext.device.logicalDevice,
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            vkContext.device.graphicsFamily,
            frames[i].commandPool);

        allocateVkCommandBuffer(
            frames[i].mainCommandBuffer,
            vkContext.device.logicalDevice,
            frames[i].commandPool);

#ifndef NDEBUG
        std::string commandPoolName{ "CommandPool Frame " + std::to_string(i) };
        setDebugUtilsObjectNameEXT(
            vkContext.device.logicalDevice,
            VK_OBJECT_TYPE_COMMAND_POOL,
            reinterpret_cast<std::uint64_t>(frames[i].commandPool),
            commandPoolName.c_str());

        std::string commandBufferName{ "CommandBuffer Frame " +
                                       std::to_string(i) };
        setDebugUtilsObjectNameEXT(
            vkContext.device.logicalDevice,
            VK_OBJECT_TYPE_COMMAND_BUFFER,
            reinterpret_cast<std::uint64_t>(frames[i].mainCommandBuffer),
            commandBufferName.c_str());
#endif
    }

    createVkCommandPool(
        vkContext.device.logicalDevice,
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        vkContext.device.graphicsFamily,
        oneTimeSubmitCommandPool);

    allocateVkCommandBuffer(
        oneTimeSubmitCommandBuffer,
        vkContext.device.logicalDevice,
        oneTimeSubmitCommandPool);

#ifndef NDEBUG
    setDebugUtilsObjectNameEXT(
        vkContext.device.logicalDevice,
        VK_OBJECT_TYPE_COMMAND_POOL,
        reinterpret_cast<std::uint64_t>(oneTimeSubmitCommandPool),
        "CommandPool One-Time");

    setDebugUtilsObjectNameEXT(
        vkContext.device.logicalDevice,
        VK_OBJECT_TYPE_COMMAND_BUFFER,
        reinterpret_cast<std::uint64_t>(oneTimeSubmitCommandBuffer),
        "CommandBuffer One-Time");
#endif
}

void Engine::InitDrawObjects()
{
    VkExtent3D drawImageExtent = {
        .width = vkContext.window.extent.width,
        .height = vkContext.window.extent.height,
        .depth = 1,
    };

    drawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
    drawImage.imageExtent = drawImageExtent;

    create2DImage(
        vkContext.allocator,
        drawImage,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | // todo remove
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT |
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    create2DImageView(
        vkContext.device.logicalDevice,
        drawImage,
        VK_IMAGE_ASPECT_COLOR_BIT);

    depthImage.imageFormat = VK_FORMAT_D32_SFLOAT;
    depthImage.imageExtent = drawImageExtent;

    create2DImage(
        vkContext.allocator,
        depthImage,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    create2DImageView(
        vkContext.device.logicalDevice,
        depthImage,
        VK_IMAGE_ASPECT_DEPTH_BIT);

#ifndef NDEBUG
    setDebugUtilsObjectNameEXT(
        vkContext.device.logicalDevice,
        VK_OBJECT_TYPE_IMAGE,
        reinterpret_cast<std::uint64_t>(drawImage.image),
        "Image Draw Color");

    setDebugUtilsObjectNameEXT(
        vkContext.device.logicalDevice,
        VK_OBJECT_TYPE_IMAGE,
        reinterpret_cast<std::uint64_t>(depthImage.image),
        "Image Draw Depth");

    setDebugUtilsObjectNameEXT(
        vkContext.device.logicalDevice,
        VK_OBJECT_TYPE_IMAGE_VIEW,
        reinterpret_cast<std::uint64_t>(drawImage.imageView),
        "Image View Draw Color");

    setDebugUtilsObjectNameEXT(
        vkContext.device.logicalDevice,
        VK_OBJECT_TYPE_IMAGE_VIEW,
        reinterpret_cast<std::uint64_t>(depthImage.imageView),
        "Image View Draw Depth");
#endif
}
}
