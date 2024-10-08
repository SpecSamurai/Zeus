#include "Application.hpp"

#include "math/Matrix4x4.hpp"
#include "renderer/Scene.hpp"
#include "vulkan/DescriptorWriter.hpp"
#include "vulkan/PipelineBuilder.hpp"
#include "vulkan/vulkan_buffer.hpp"
#include "vulkan/vulkan_pipeline.hpp"

#include <core/Timer.hpp>
#include <core/logger.hpp>
#include <cstddef>
#include <cstring>
#include <math/definitions.hpp>
#include <renderer/Renderer.hpp>
#include <vector>
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

#include <array>
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

    InitCompute();
    InitMesh();

    // uiManager.Init(m_window, m_vkContext);
}

void Application::Run()
{
    while (!glfwWindowShouldClose(m_window.handle))
    {
        Timer timer;
        timer.Start();

        glfwPollEvents();

        if (m_window.invalidExtent)
        {
            glfwWaitEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        if (m_renderer.ResizeRequired())
        {
            m_renderer.ResizeDrawObjects(m_window.extent);
        }

        // uiManager.ConfigureFrame();

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
    debug("Shutting down application");
    vkDeviceWaitIdle(m_vkContext.GetDevice().logicalDevice);

    // uiManager.Destroy(m_vkContext.GetDevice().logicalDevice);

    m_renderer.Destroy();
    m_vkContext.Destroy();
    m_window.Destroy();
}

void Application::Draw()
{
    m_renderer.BeginFrame();

    transitionImageLayout(
        m_renderer.CurrentFrame().mainCommandBuffer,
        m_renderer.drawImage.image,
        m_renderer.drawImage.imageFormat,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_GENERAL);

    transitionImageLayout(
        m_renderer.CurrentFrame().mainCommandBuffer,
        m_renderer.depthImage.image,
        m_renderer.depthImage.imageFormat,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

    // cmdClearColorImage(
    //     m_renderer.CurrentFrame().mainCommandBuffer,
    //     m_renderer.drawImage.image,
    //     Vector4f(0.f, 0.f, 0.5f, 1.f),
    //     VK_IMAGE_LAYOUT_GENERAL);

    DrawCompute();

    DrawTriangle();

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

    blitImage(
        m_renderer.CurrentFrame().mainCommandBuffer,
        m_renderer.drawImage.image,
        m_vkContext.GetSwapchain()
            .images[m_renderer.CurrentSwapchainImageIndex()],
        m_renderer.drawExtent,
        m_vkContext.GetSwapchain().extent);

    // transitionImageLayout(
    //     m_renderer.CurrentFrame().mainCommandBuffer,
    //     m_vkContext.GetSwapchain()
    //         .images[m_renderer.CurrentSwapchainImageIndex()],
    //     m_vkContext.GetSwapchain().imageFormat,
    //     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    //     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    //
    // VkRenderingAttachmentInfo colorAttachment{
    //     createColorAttachmentInfo(
    //         m_vkContext.GetSwapchain()
    //             .imageViews[m_renderer.CurrentSwapchainImageIndex()],
    //         VK_IMAGE_LAYOUT_GENERAL),
    // };

    // uiManager.Draw(
    //     m_renderer.CurrentFrame().mainCommandBuffer,
    //     colorAttachment,
    //     m_vkContext.GetSwapchain().extent);

    transitionImageLayout(
        m_renderer.CurrentFrame().mainCommandBuffer,
        m_vkContext.GetSwapchain()
            .images[m_renderer.CurrentSwapchainImageIndex()],
        m_vkContext.GetSwapchain().imageFormat,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    m_renderer.EndFrame();
}

void Application::DrawCompute()
{
    vkCmdBindPipeline(
        m_renderer.CurrentFrame().mainCommandBuffer,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        computeEffect.pipeline);

    vkCmdBindDescriptorSets(
        m_renderer.CurrentFrame().mainCommandBuffer,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        computeEffect.layout,
        0,
        1,
        &_drawImageDescriptors,
        0,
        nullptr);

    vkCmdPushConstants(
        m_renderer.CurrentFrame().mainCommandBuffer,
        computeEffect.layout,
        VK_SHADER_STAGE_COMPUTE_BIT,
        0,
        sizeof(ComputePushConstants),
        &computeEffect.data);

    // 16x16 workgroup size so we need to divide by it
    vkCmdDispatch(
        m_renderer.CurrentFrame().mainCommandBuffer,
        std::ceil(m_renderer.drawExtent.width / 16.0),
        std::ceil(m_renderer.drawExtent.height / 16.0),
        1);
}

void Application::InitCompute()
{
    const std::vector<VkDescriptorPoolSize> poolSizes{
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 10 }
    };

    computeDescriptorAllocator.init(
        m_vkContext.GetDevice().logicalDevice,
        10,
        poolSizes);

    {
        DescriptorLayoutBuilder descriptorLayoutBuilder;
        descriptorLayoutBuilder.addBinding(
            0,
            VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            VK_SHADER_STAGE_COMPUTE_BIT);
        _drawImageDescriptorLayout = descriptorLayoutBuilder.build(
            m_vkContext.GetDevice().logicalDevice);
    }

    _drawImageDescriptors = computeDescriptorAllocator.allocate(
        m_vkContext.GetDevice().logicalDevice,
        _drawImageDescriptorLayout);

    {
        DescriptorWriter descriptorWriter;
        descriptorWriter.writeDescriptorImage(
            VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            0,
            m_renderer.drawImage.imageView,
            VK_NULL_HANDLE,
            VK_IMAGE_LAYOUT_GENERAL);

        descriptorWriter.updateDescriptorSet(
            m_vkContext.GetDevice().logicalDevice,
            _drawImageDescriptors);
    }

    VkShaderModule computeShader;
    loadShader(
        computeShader,
        m_vkContext.GetDevice().logicalDevice,
        "../engine/shaders/gradient_color.comp.spv");

    VkPushConstantRange pushConstant;
    pushConstant.offset = 0;
    pushConstant.size = sizeof(ComputePushConstants);
    pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    computeEffect.data = {};
    computeEffect.data.data1 = Vector4f(1, 0, 0, 1);
    computeEffect.data.data2 = Vector4f(0, 0, 1, 1);

    createVkPipelineLayout(
        computeEffect.layout,
        m_vkContext.GetDevice().logicalDevice,
        1,
        &_drawImageDescriptorLayout,
        1,
        &pushConstant);

    createComputePipeline(
        computeEffect.pipeline,
        m_vkContext.GetDevice().logicalDevice,
        computeEffect.layout,
        computeShader);

    vkDestroyShaderModule(
        m_vkContext.GetDevice().logicalDevice,
        computeShader,
        allocationCallbacks.get());
}

void Application::InitMesh()
{
    VkShaderModule vertShader;
    loadShader(
        vertShader,
        m_vkContext.GetDevice().logicalDevice,
        "../engine/shaders/basic.vert.spv");

    VkShaderModule fragShader;
    loadShader(
        fragShader,
        m_vkContext.GetDevice().logicalDevice,
        "../engine/shaders/basic.frag.spv");

    VkPushConstantRange pushConstantRange{};
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(MeshPushConstants);
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    createVkPipelineLayout(
        meshPipelineLayout,
        m_vkContext.GetDevice().logicalDevice,
        0,
        nullptr,
        1,
        &pushConstantRange);

    {
        PipelineBuilder builder;
        builder.addShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertShader);
        builder.addShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fragShader);
        builder.setPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        builder.setPolygonMode(VK_POLYGON_MODE_FILL);
        builder.setCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
        builder.disableBlending();
        builder.disableDepthTest();
        builder.disableMiltisampling();
        builder.setDynamicStates(
            { VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_VIEWPORT });

        builder.dynamicRendering();
        builder.setColorAttachmentFormat(m_renderer.drawImage.imageFormat);
        builder.setDepthFormat(VK_FORMAT_UNDEFINED);

        meshPipeline = builder.build(
            m_vkContext.GetDevice().logicalDevice,
            meshPipelineLayout);
    }

    vkDestroyShaderModule(
        m_vkContext.GetDevice().logicalDevice,
        vertShader,
        allocationCallbacks.get());

    vkDestroyShaderModule(
        m_vkContext.GetDevice().logicalDevice,
        fragShader,
        allocationCallbacks.get());

    std::array<Vertex, 4> rect_vertices;

    rect_vertices[0].position = { 0.5, -0.5, 0 };
    rect_vertices[1].position = { 0.5, 0.5, 0 };
    rect_vertices[2].position = { -0.5, -0.5, 0 };
    rect_vertices[3].position = { -0.5, 0.5, 0 };

    rect_vertices[0].color = { 0, 0, 0, 1 };
    rect_vertices[1].color = { 0.5, 0.5, 0.5, 1 };
    rect_vertices[2].color = { 1, 0, 0, 1 };
    rect_vertices[3].color = { 0, 1, 0, 1 };

    std::array<uint32_t, 6> rect_indices;

    rect_indices[0] = 0;
    rect_indices[1] = 1;
    rect_indices[2] = 2;

    rect_indices[3] = 2;
    rect_indices[4] = 1;
    rect_indices[5] = 3;

    rectangle = UploadMesh(rect_vertices, rect_indices);
}

void Application::DrawTriangle()
{
    // transitionImageLayout(
    //     m_renderer.CurrentFrame().mainCommandBuffer,
    //     m_renderer.drawImage.image,
    //     m_renderer.drawImage.imageFormat,
    //     VK_IMAGE_LAYOUT_GENERAL,
    //     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    VkRenderingAttachmentInfo colorAttachmentInfo = createColorAttachmentInfo(
        m_renderer.drawImage.imageView,
        VK_IMAGE_LAYOUT_GENERAL);

    cmdBeginRendering(
        m_renderer.CurrentFrame().mainCommandBuffer,
        m_renderer.drawExtent,
        1,
        &colorAttachmentInfo);

    vkCmdBindPipeline(
        m_renderer.CurrentFrame().mainCommandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        meshPipeline);

    cmdSetVkViewport(
        m_renderer.CurrentFrame().mainCommandBuffer,
        m_renderer.drawExtent.width,
        m_renderer.drawExtent.height);

    cmdSetVkScissor(
        m_renderer.CurrentFrame().mainCommandBuffer,
        m_renderer.drawExtent);

    MeshPushConstants push_constants;
    push_constants.modelMatrix = Matrix4x4f(1.f);
    push_constants.vertexBufferAddress = rectangle.vertexBufferAddress;

    vkCmdPushConstants(
        m_renderer.CurrentFrame().mainCommandBuffer,
        meshPipelineLayout,
        VK_SHADER_STAGE_VERTEX_BIT,
        0,
        sizeof(MeshPushConstants),
        &push_constants);

    vkCmdBindIndexBuffer(
        m_renderer.CurrentFrame().mainCommandBuffer,
        rectangle.indexBuffer.buffer,
        0,
        VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(
        m_renderer.CurrentFrame().mainCommandBuffer,
        6,
        1,
        0,
        0,
        0);

    vkCmdEndRendering(m_renderer.CurrentFrame().mainCommandBuffer);
}

MeshBuffers Application::UploadMesh(
    std::span<Vertex> vertices,
    std::span<std::uint32_t> indices)
{
    const VkDeviceSize vertexBufferSize = vertices.size() * sizeof(Vertex);
    const VkDeviceSize indexBufferSize = indices.size() * sizeof(std::uint32_t);

    MeshBuffers meshBuffers;

    createBuffer(
        m_vkContext.GetAllocator(),
        meshBuffers.vertexBuffer,
        vertexBufferSize,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    meshBuffers.vertexBufferAddress = getBufferDeviceAddress(
        m_vkContext.GetDevice().logicalDevice,
        meshBuffers.vertexBuffer.buffer);

    createBuffer(
        m_vkContext.GetAllocator(),
        meshBuffers.indexBuffer,
        indexBufferSize,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    Buffer staging;
    createBuffer(
        m_vkContext.GetAllocator(),
        staging,
        vertexBufferSize + indexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        true);

    void* data{ staging.info.pMappedData };

    memcpy(data, vertices.data(), vertexBufferSize);
    memcpy(
        reinterpret_cast<char*>(data) + vertexBufferSize,
        indices.data(),
        indexBufferSize);
    // memcpy((char*)data + vertexBufferSize, indices.data(), indexBufferSize);

    m_vkContext.CmdImmediateSubmit([&](VkCommandBuffer commandBuffer) {
        // VkBufferCopy vertexBufferCopy{};
        // vertexBufferCopy.srcOffset = 0;
        // vertexBufferCopy.dstOffset = 0;
        // vertexBufferCopy.size = vertexBufferSize;
        //
        // vkCmdCopyBuffer(
        //     commandBuffer,
        //     staging.buffer,
        //     meshBuffers.vertexBuffer.buffer,
        //     1,
        //     &vertexBufferCopy);

        // VkBufferCopy indexBufferCopy{};
        // indexBufferCopy.srcOffset = vertexBufferSize;
        // indexBufferCopy.dstOffset = 0;
        // indexBufferCopy.size = indexBufferSize;
        //
        // vkCmdCopyBuffer(
        //     commandBuffer,
        //     staging.buffer,
        //     meshBuffers.indexBuffer.buffer,
        //     1,
        //     &indexBufferCopy);

        cmdCopyBuffer(
            commandBuffer,
            staging.buffer,
            meshBuffers.vertexBuffer.buffer,
            0,
            0,
            vertexBufferSize);

        cmdCopyBuffer(
            commandBuffer,
            staging.buffer,
            meshBuffers.indexBuffer.buffer,
            vertexBufferSize,
            0,
            indexBufferSize);
    });

    vmaDestroyBuffer(
        m_vkContext.GetAllocator(),
        staging.buffer,
        staging.allocation);

    return meshBuffers;
}
}
