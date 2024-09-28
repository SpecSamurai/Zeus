#include "Engine.hpp"
#include "core/logger.hpp"
#include "vulkan/vulkan_command.hpp"
#include "vulkan/vulkan_debug.hpp"
#include "vulkan/vulkan_memory.hpp"
#include "vulkan/vulkan_sync.hpp"
#include "vulkan/vulkan_types.hpp"

#include <chrono>
#include <cstdint>
#include <string>
#include <thread>
#include <vulkan/vulkan_core.h>

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
}

void Engine::Run()
{
    while (!glfwWindowShouldClose(vkContext.window.handle))
    {
        // auto start = std::chrono::system_clock::now();
        glfwPollEvents();

        // // Handle events on queue
        // while (SDL_PollEvent(&e) != 0)
        // {
        //     // close the window when user alt-f4s or clicks the X button
        //     if (e.type == SDL_QUIT)
        //         bQuit = true;
        //
        //     if (e.type == SDL_WINDOWEVENT)
        //     {
        //
        //         if (e.window.event == SDL_WINDOWEVENT_RESIZED)
        //         {
        //             resize_requested = true;
        //         }
        //         if (e.window.event == SDL_WINDOWEVENT_MINIMIZED)
        //         {
        //             freeze_rendering = true;
        //         }
        //         if (e.window.event == SDL_WINDOWEVENT_RESTORED)
        //         {
        //             freeze_rendering = false;
        //         }
        //     }
        //
        //     mainCamera.processSDLEvent(e);
        //     ImGui_ImplSDL2_ProcessEvent(&e);
        // }

        if (vkContext.window.invalidExtent) // freeze_rendering)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        if (vkContext.window.resized)
        {
            vkContext.RecreateSwapchain();
        }

        // // imgui new frame
        // ImGui_ImplVulkan_NewFrame();
        // ImGui_ImplSDL2_NewFrame();
        //
        // ImGui::NewFrame();
        //
        // ImGui::Begin("Stats");
        //
        // ImGui::Text("frametime %f ms", stats.frametime);
        // ImGui::Text("drawtime %f ms", stats.mesh_draw_time);
        // ImGui::Text("triangles %i", stats.triangle_count);
        // ImGui::Text("draws %i", stats.drawcall_count);
        // ImGui::End();
        //
        // if (ImGui::Begin("background"))
        // {
        //
        //     ComputeEffect& selected =
        //         backgroundEffects[currentBackgroundEffect];
        //
        //     ImGui::Text("Selected effect: ", selected.name);
        //
        //     ImGui::SliderInt(
        //         "Effect Index",
        //         &currentBackgroundEffect,
        //         0,
        //         backgroundEffects.size() - 1);
        //
        //     ImGui::InputFloat4("data1", (float*)&selected.data.data1);
        //     ImGui::InputFloat4("data2", (float*)&selected.data.data2);
        //     ImGui::InputFloat4("data3", (float*)&selected.data.data3);
        //     ImGui::InputFloat4("data4", (float*)&selected.data.data4);
        //
        //     ImGui::End();
        // }
        //
        // ImGui::Render();
        //
        // // imgui commands
        // // ImGui::ShowDemoWindow();
        //
        // update_scene();

        Draw();

        // auto end = std::chrono::system_clock::now();
        // auto elapsed =
        //     std::chrono::duration_cast<std::chrono::microseconds>(end -
        //     start);
        //
        // stats.frametime = elapsed.count() / 1000.f;
    }
}

void Engine::Shutdown()
{
    debug("Shutting down engine");
    vkDeviceWaitIdle(vkContext.device.logicalDevice);

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
    currentFrame = (currentFrame + 1) % vkContext.swapchain.maxConcurrentFrames;
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
}
