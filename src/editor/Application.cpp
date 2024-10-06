#include "Application.hpp"

#include <core/Timer.hpp>
#include <core/logger.hpp>
#include <renderer/Renderer.hpp>
#include <vulkan/DescriptorLayoutBuilder.hpp>
#include <vulkan/VulkanContext.hpp>
#include <vulkan/vulkan_command.hpp>
#include <vulkan/vulkan_debug.hpp>
#include <vulkan/vulkan_dynamic_rendering.hpp>
#include <vulkan/vulkan_image.hpp>
#include <vulkan/vulkan_memory.hpp>
#include <vulkan/vulkan_shader.hpp>
#include <vulkan/vulkan_sync.hpp>

#include <vulkan/vulkan_core.h>

#include <chrono>
#include <cstdint>
#include <thread>

namespace Zeus
{
Application::Application(
    std::uint32_t width,
    std::uint32_t height,
    const char* title)
    : m_window(width, height, title),
      m_vkContext(m_window),
      m_renderer(m_window, m_vkContext)
{
}

void Application::Init()
{
    m_window.Init();
    m_vkContext.Init();
    m_renderer.Init();

    InitDescriptors();

    uiManager.Init(m_window, m_vkContext);
}

void Application::Run()
{
    while (!glfwWindowShouldClose(m_window.handle))
    {
        Timer timer;
        timer.Start();
        glfwPollEvents();

        if (m_window.invalidExtent) // freeze_rendering)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        if (m_window.resized)
        {
            RecreateSwapchain();
        }

        uiManager.ConfigureFrame();

        // imgui commands
        // updateScene();

        Draw();

        timer.End();
        statistics.frametime = timer.GetMilliseconds();
        statistics.fps = 1000.f / statistics.frametime;
    }
}

void Application::Shutdown()
{
    debug("Shutting down engine");
    vkDeviceWaitIdle(m_vkContext.GetDevice().logicalDevice);

    uiManager.Destroy(m_vkContext.GetDevice().logicalDevice);

    debug("Destroying commands");
    vkDestroyCommandPool(
        m_vkContext.GetDevice().logicalDevice,
        oneTimeSubmitCommandPool,
        allocationCallbacks.get());

    debug("Destroying sync objects");

    vkDestroyFence(
        m_vkContext.GetDevice().logicalDevice,
        oneTimeSubmitFence,
        allocationCallbacks.get());

    m_renderer.Destroy();
    m_vkContext.Destroy();
    m_window.Destroy();
}

void Application::cmdOneTimeSubmit(
    std::function<void(VkCommandBuffer cmd)>&& function)
{
    VKCHECK(
        vkResetFences(
            m_vkContext.GetDevice().logicalDevice,
            1,
            &oneTimeSubmitFence),
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
        m_vkContext.GetDevice().transferQueue,
        oneTimeSubmitFence,
        0,
        nullptr,
        1,
        &submitInfo,
        0,
        nullptr);

    VKCHECK(
        vkWaitForFences(
            m_vkContext.GetDevice().logicalDevice,
            1,
            &oneTimeSubmitFence,
            VK_TRUE,
            UINT64_MAX),
        "Failed to wait for fence");
}

void Application::Draw()
{
    m_renderer.BeginFrame();

    // drawMain(getCurrentFrame().mainCommandBuffer);

    // VkClearColorValue clearValue;
    // clearValue = { { 0.0f, 0.0f, 1.f, 1.0f } };
    //
    // VkImageSubresourceRange subImage{};
    // subImage.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    // subImage.baseMipLevel = 0;
    // subImage.levelCount = VK_REMAINING_MIP_LEVELS;
    // subImage.baseArrayLayer = 0;
    // subImage.layerCount = VK_REMAINING_ARRAY_LAYERS;
    //
    // vkCmdClearColorImage(
    //     getCurrentFrame().mainCommandBuffer,
    //     drawImage.image,
    //     VK_IMAGE_LAYOUT_GENERAL,
    //     &clearValue,
    //     1,
    //     &subImage);

    // bind the gradient drawing compute pipeline
    // ComputeEffect& effect = backgroundEffects[0];
    //
    // vkCmdBindPipeline(
    //     getCurrentFrame().mainCommandBuffer,
    //     VK_PIPELINE_BIND_POINT_COMPUTE,
    //     effect.pipeline);

    // bind the descriptor set containing the draw image for the compute
    // pipeline
    // vkCmdBindDescriptorSets(
    //     getCurrentFrame().mainCommandBuffer,
    //     VK_PIPELINE_BIND_POINT_COMPUTE,
    //     effect.layout,
    //     0,
    //     1,
    //     &_drawImageDescriptors,
    //     0,
    //     nullptr);
    //
    // vkCmdPushConstants(
    //     getCurrentFrame().mainCommandBuffer,
    //     effect.layout,
    //     VK_SHADER_STAGE_COMPUTE_BIT,
    //     0,
    //     sizeof(ComputePushConstants),
    //     &effect.data);

    // execute the compute pipeline dispatch. We are using 16x16 workgroup size
    // so we need to divide by it
    // vkCmdDispatch(
    //     getCurrentFrame().mainCommandBuffer,
    //     std::ceil(drawExtent.width / 16.0),
    //     std::ceil(drawExtent.height / 16.0),
    //     1);

    transitionImageLayout(
        m_renderer.CurrentFrame().mainCommandBuffer,
        m_renderer.drawImage.image,
        m_renderer.drawImage.imageFormat,
        VK_IMAGE_LAYOUT_GENERAL,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    transitionImageLayout(
        m_renderer.CurrentFrame().mainCommandBuffer,
        m_vkContext.GetSwapchain()
            .images[m_renderer.CurrentSwapchainImageIndex()],
        m_vkContext.GetSwapchain().imageFormat,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // VkExtent2D extent;
    // extent.height = _windowExtent.height;
    // extent.width = _windowExtent.width;
    // extent.depth = 1;

    blitImage(
        m_renderer.CurrentFrame().mainCommandBuffer,
        m_renderer.drawImage.image,
        m_vkContext.GetSwapchain()
            .images[m_renderer.CurrentSwapchainImageIndex()],
        m_renderer.drawExtent,
        m_vkContext.GetSwapchain().extent);

    transitionImageLayout(
        m_renderer.CurrentFrame().mainCommandBuffer,
        m_vkContext.GetSwapchain()
            .images[m_renderer.CurrentSwapchainImageIndex()],
        m_vkContext.GetSwapchain().imageFormat,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    // drawUI(
    //     getCurrentFrame().mainCommandBuffer,
    //     m_vkContext.swapchain.imageViews[swapchainImageIndex]);

    // VkRenderingAttachmentInfo colorAttachment = vkinit::attachment_info(
    //     targetImageView,
    //     nullptr,
    //     VK_IMAGE_LAYOUT_GENERAL);
    // VkRenderingInfo renderInfo =
    //     vkinit::rendering_info(_windowExtent, &colorAttachment, nullptr);

    // vkCmdBeginRendering(cmd, &renderInfo);

    VkRenderingAttachmentInfo colorAttachment{
        createColorAttachmentInfo(
            m_vkContext.GetSwapchain()
                .imageViews[m_renderer.CurrentSwapchainImageIndex()],
            VK_IMAGE_LAYOUT_GENERAL),
    };

    uiManager.Draw(
        m_renderer.CurrentFrame().mainCommandBuffer,
        colorAttachment,
        m_window.extent);

    transitionImageLayout(
        m_renderer.CurrentFrame().mainCommandBuffer,
        m_vkContext.GetSwapchain()
            .images[m_renderer.CurrentSwapchainImageIndex()],
        m_vkContext.GetSwapchain().imageFormat,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    m_renderer.EndFrame();
}

void Application::RecreateSwapchain()
{
    // Handles minimazation
    // int width = 0, height = 0;
    // glfwGetFramebufferSize(window, &width, &height);
    // while (width == 0 || height == 0)
    // {
    //     glfwGetFramebufferSize(window, &width, &height);
    //     glfwWaitEvents();
    // }

    vkDeviceWaitIdle(m_vkContext.GetDevice().logicalDevice);

    m_vkContext.ResizeSwapchain(m_window.extent);
    m_renderer.ResizeDrawObjects(m_window.extent);

    m_window.resized = false;
}

void Application::InitSyncObjects()
{
    VKCHECK(
        createVkFence(
            m_vkContext.GetDevice().logicalDevice,
            true,
            oneTimeSubmitFence),
        "Failed to create oneTimeSubmit fence.");

#ifndef NDEBUG
    setDebugUtilsObjectNameEXT(
        m_vkContext.GetDevice().logicalDevice,
        VK_OBJECT_TYPE_FENCE,
        reinterpret_cast<std::uint64_t>(oneTimeSubmitFence),
        "Fence One-Time Submit");
#endif
}

void Application::InitCommands()
{
    createVkCommandPool(
        m_vkContext.GetDevice().logicalDevice,
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        m_vkContext.GetDevice().graphicsFamily,
        oneTimeSubmitCommandPool);

    allocateVkCommandBuffer(
        oneTimeSubmitCommandBuffer,
        m_vkContext.GetDevice().logicalDevice,
        oneTimeSubmitCommandPool);

#ifndef NDEBUG
    setDebugUtilsObjectNameEXT(
        m_vkContext.GetDevice().logicalDevice,
        VK_OBJECT_TYPE_COMMAND_POOL,
        reinterpret_cast<std::uint64_t>(oneTimeSubmitCommandPool),
        "CommandPool One-Time");

    setDebugUtilsObjectNameEXT(
        m_vkContext.GetDevice().logicalDevice,
        VK_OBJECT_TYPE_COMMAND_BUFFER,
        reinterpret_cast<std::uint64_t>(oneTimeSubmitCommandBuffer),
        "CommandBuffer One-Time");
#endif
}

void Application::InitDescriptors()
{
    // create a descriptor pool
    std::vector<VkDescriptorPoolSize> sizes = {
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3 },
    };

    globalDescriptorAllocator.init(
        m_vkContext.GetDevice().logicalDevice,
        10,
        sizes);

    // _mainDeletionQueue.push_function([&]() {
    //     vkDestroyDescriptorPool(
    //         _device,
    //         globalDescriptorAllocator.pool,
    //         nullptr);
    // });

    {
        DescriptorLayoutBuilder builder;
        builder.addBinding(
            0,
            VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            VK_SHADER_STAGE_COMPUTE_BIT);

        _drawImageDescriptorLayout =
            builder.build(m_vkContext.GetDevice().logicalDevice);
    }
    // {
    //     DescriptorLayoutBuilder builder;
    //     builder.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    //     builder.add_binding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    //
    //     VkDescriptorSetLayoutBindingFlagsCreateInfo bindFlags = {
    //         .sType =
    //             VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
    //         .pNext = nullptr
    //     };
    //
    //     std::array<VkDescriptorBindingFlags, 2> flagArray{
    //         0,
    //         VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT |
    //             VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT
    //     };
    //
    //     builder.bindings[1].descriptorCount = 4048;
    //
    //     bindFlags.bindingCount = 2;
    //     bindFlags.pBindingFlags = flagArray.data();
    //
    //     _gpuSceneDataDescriptorLayout = builder.build(
    //         _device,
    //         VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
    //         &bindFlags);
    // }

    // _mainDeletionQueue.push_function([&]() {
    //     vkDestroyDescriptorSetLayout(
    //         _device,
    //         _drawImageDescriptorLayout,
    //         nullptr);
    //     vkDestroyDescriptorSetLayout(
    //         _device,
    //         _gpuSceneDataDescriptorLayout,
    //         nullptr);
    // });

    // _drawImageDescriptors = globalDescriptorAllocator.allocate(
    //     m_vkContext.device.logicalDevice,
    //     _drawImageDescriptorLayout);
    // {
    //     DescriptorWriter writer;
    //     writer.writeDescriptorImage(
    //         VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    //         0,
    //         m_renderer.drawImage.imageView,
    //         VK_NULL_HANDLE,
    //         VK_IMAGE_LAYOUT_GENERAL);
    //     writer.updateDescriptorSet(
    //         m_vkContext.device.logicalDevice,
    //         _drawImageDescriptors);
    // }
    //
    // for (int i = 0; i < FRAME_OVERLAP; i++)
    // {
    //     // create a descriptor pool
    //     std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> frame_sizes =
    //     {
    //         { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3 },
    //         { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3 },
    //         { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 },
    //         { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4 },
    //     };
    //
    //     _frames[i]._frameDescriptors = DescriptorAllocatorGrowable{};
    //     _frames[i]._frameDescriptors.init(_device, 1000, frame_sizes);
    //     _mainDeletionQueue.push_function(
    //         [&, i]() { _frames[i]._frameDescriptors.destroy_pools(_device);
    //         });
    // }

    //*****************************************************
    // VkPipelineLayoutCreateInfo computeLayout{};
    // computeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    // computeLayout.pNext = nullptr;
    // computeLayout.pSetLayouts = &_drawImageDescriptorLayout;
    // computeLayout.setLayoutCount = 1;

    // VkPushConstantRange pushConstant{};
    // pushConstant.offset = 0;
    // pushConstant.size = sizeof(ComputePushConstants);
    // pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    // computeLayout.pPushConstantRanges = &pushConstant;
    // computeLayout.pushConstantRangeCount = 1;

    // VK_CHECK(vkCreatePipelineLayout(
    //     _device,
    //     &computeLayout,
    //     nullptr,
    //     &_gradientPipelineLayout));

    // createVkPipelineLayout(
    //     _gradientPipelineLayout,
    //     m_vkContext.device.logicalDevice,
    //     1,
    //     &_drawImageDescriptorLayout,
    //     1,
    //     &pushConstant);

    VkShaderModule gradientShader;
    loadShader(
        gradientShader,
        m_vkContext.GetDevice().logicalDevice,
        "../engine/shaders/gradient_color.comp.spv");
    // if (!vkutil::load_shader_module(, _device, &gradientShader))
    // {
    //     fmt::print("Error when building the compute shader \n");
    // }

    // VkShaderModule skyShader;
    // loadShader(
    //     skyShader,
    //     m_vkContext.device.logicalDevice,
    //     "../../shaders/sky.comp.spv");

    // if (!vkutil::load_shader_module(
    //         ,
    //         _device,
    //         &skyShader))
    // {
    //     fmt::print("Error when building the compute shader\n");
    // }

    // VkPipelineShaderStageCreateInfo stageinfo{};
    // stageinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    // stageinfo.pNext = nullptr;
    // stageinfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    // stageinfo.module = gradientShader;
    // stageinfo.pName = "main";

    // VkComputePipelineCreateInfo computePipelineCreateInfo{};
    // computePipelineCreateInfo.sType =
    //     VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    // computePipelineCreateInfo.pNext = nullptr;
    // computePipelineCreateInfo.layout = _gradientPipelineLayout;
    // computePipelineCreateInfo.stage = stageinfo;

    // ComputeEffect gradient;
    // gradient.layout = _gradientPipelineLayout;
    // gradient.name = "gradient";
    // gradient.data = {};

    // default colors
    // gradient.data.data1 = Vector4f(1, 0, 0, 1);
    // gradient.data.data2 = Vector4f(0, 0, 1, 1);

    // VK_CHECK(vkCreateComputePipelines(
    //     _device,
    //     VK_NULL_HANDLE,
    //     1,
    //     &computePipelineCreateInfo,
    //     nullptr,
    //     &gradient.pipeline));

    // createComputePipeline(
    //     gradient.pipeline,
    //     m_vkContext.device.logicalDevice,
    //     _gradientPipelineLayout,
    //     gradientShader);

    // change the shader module only to create the sky shader
    // computePipelineCreateInfo.stage.module = skyShader;

    // ComputeEffect sky;
    // sky.layout = _gradientPipelineLayout;
    // sky.name = "sky";
    // sky.data = {};
    // // default sky parameters
    // sky.data.data1 = glm::vec4(0.1, 0.2, 0.4, 0.97);
    //
    // VK_CHECK(vkCreateComputePipelines(
    //     _device,
    //     VK_NULL_HANDLE,
    //     1,
    //     &computePipelineCreateInfo,
    //     nullptr,
    //     &sky.pipeline));

    // add the 2 background effects into the array
    // backgroundEffects.push_back(gradient);
    // backgroundEffects.push_back(sky);

    // destroy structures properly
    // vkDestroyShaderModule(_device, gradientShader, nullptr);
    // vkDestroyShaderModule(_device, skyShader, nullptr);
    // _mainDeletionQueue.push_function([&]() {
    //     vkDestroyPipelineLayout(_device, _gradientPipelineLayout, nullptr);
    //     vkDestroyPipeline(_device, sky.pipeline, nullptr);
    //     vkDestroyPipeline(_device, gradient.pipeline, nullptr);
    // });
}
}
