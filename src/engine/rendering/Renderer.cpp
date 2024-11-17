#include "Renderer.hpp"

#include "gltf/GltfLoader.hpp"
#include "logging/logger.hpp"
#include "math/transformations.hpp"
#include "rendering/renderer_types.hpp"
#include "vulkan/BlendState.hpp"
#include "vulkan/Buffer.hpp"
#include "vulkan/CommandBuffer.hpp"
#include "vulkan/Definitions.hpp"
#include "vulkan/DescriptorSetLayout.hpp"
#include "vulkan/DescriptorWriter.hpp"
#include "vulkan/Pipeline.hpp"
#include "vulkan/PipelineState.hpp"
#include "vulkan/RasterizationState.hpp"
#include "vulkan/Shader.hpp"
#include "vulkan/Swapchain.hpp"
#include "vulkan/VkContext.hpp"
#include "vulkan/rhi/vulkan_debug.hpp"
#include "vulkan/rhi/vulkan_dynamic_rendering.hpp"
#include "vulkan/rhi/vulkan_image.hpp"
#include "window/Window.hpp"

#include <vulkan/vulkan_core.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <span>

namespace Zeus
{
Renderer::Renderer(Window& window) : m_window{ window }
{
}

void Renderer::Init()
{
    m_swapchain = std::make_unique<Swapchain>(Swapchain(
        VkContext::GetSurface(),
        m_window.GetWidth(),
        m_window.GetHeight(),
        VK_PRESENT_MODE_MAILBOX_KHR,
        2));

    m_frames.resize(m_swapchain->GetFramesCount());

    InitCommands();
    VkExtent2D e{ m_window.GetWidth(), m_window.GetHeight() };
    InitDrawObjects(e);
    InitDescriptors();

    InitCompute();
    InitMesh();
}

void Renderer::Destroy()
{
    LOG_DEBUG("Destroying Renderer");
    drawImage.Destroy();
    depthImage.Destroy();

    for (std::size_t i{ 0 }; i < m_swapchain->GetFramesCount(); ++i)
    {
        m_frames[i].commandPool.Destroy();
    }

    computeDescriptorAllocator.Destroy();
    sceneDataDescriptorSetLayout.Destroy();
    drawImageDescriptorLayout.Destroy();

    m_swapchain->Destroy();

    // _meshPipeline->Destroy();
    // computeEffect.pipeline->Destroy();

    rectangle.vertexBuffer->Destroy();
    rectangle.indexBuffer->Destroy();

    for (auto& test : testMeshes)
    {
        test->meshBuffers.vertexBuffer->Destroy();
        test->meshBuffers.indexBuffer->Destroy();
    }
}

void Renderer::BeginFrame()
{
    m_swapchain->AcquireNextImage();

    drawExtent = {
        .width = static_cast<std::uint32_t>(
            static_cast<float>(drawImage.GetWidth()) * m_renderScale),
        .height = static_cast<std::uint32_t>(
            static_cast<float>(drawImage.GetHeight()) * m_renderScale),
    };

    CurrentFrame().mainCommandBuffer.Reset();
    CurrentFrame().mainCommandBuffer.Begin(
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
}

void Renderer::EndFrame()
{
    CurrentFrame().mainCommandBuffer.End();

    m_swapchain->Present(CurrentFrame().mainCommandBuffer.GetHandle());
}

MeshBuffers Renderer::UploadMesh(
    std::span<Vertex> vertices,
    std::span<std::uint32_t> indices)
{
    const VkDeviceSize vertexBufferSize{ vertices.size() * sizeof(Vertex) };
    const VkDeviceSize indexBufferSize{ indices.size() *
                                        sizeof(std::uint32_t) };

    MeshBuffers meshBuffers{
        .indexBuffer = std::make_unique<Buffer>(Buffer(
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            indexBufferSize,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)),
        .vertexBuffer = std::make_unique<Buffer>(Buffer(
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
            vertexBufferSize,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)),
    };

    // createBuffer(
    //     VkContext::GetAllocator(),
    //     meshBuffers.vertexBuffer,
    //     vertexBufferSize,
    //     VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
    //     |
    //         VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
    //     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    //
    // meshBuffers.vertexBufferAddress = getBufferDeviceAddress(
    //     VkContext::GetLogicalDevice(),
    //     meshBuffers.vertexBuffer.buffer);
    //
    // createBuffer(
    //     VkContext::GetAllocator(),
    //     meshBuffers.indexBuffer,
    //     indexBufferSize,
    //     VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    //     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // Buffer staging{};
    // createBuffer(
    //     VkContext::GetAllocator(),
    //     staging,
    //     vertexBufferSize + indexBufferSize,
    //     VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    //     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
    //         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    //     true);

    Buffer staging(
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        vertexBufferSize + indexBufferSize,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        true);

    // void* data{ staging.info.pMappedData };

    // memcpy(data, vertices.data(), vertexBufferSize);
    // memcpy(
    //     reinterpret_cast<char*>(data) + vertexBufferSize,
    //     indices.data(),
    //     indexBufferSize);
    // memcpy((char*)data + vertexBufferSize, indices.data(), indexBufferSize);

    staging.Update(vertices.data(), vertexBufferSize);
    staging.Update(indices.data(), indexBufferSize, vertexBufferSize);

    VkContext::GetDevice().CmdImmediateSubmit(
        [&](const CommandBuffer& commandBuffer) {
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

            // cmdCopyBuffer(
            //     commandBuffer,
            //     staging.buffer,
            //     meshBuffers.vertexBuffer.buffer,
            //     0,
            //     0,
            //     vertexBufferSize);
            //
            // cmdCopyBuffer(
            //     commandBuffer,
            //     staging.buffer,
            //     meshBuffers.indexBuffer.buffer,
            //     vertexBufferSize,
            //     0,
            //     indexBufferSize);

            staging.CopyToBuffer(
                commandBuffer.GetHandle(),
                *meshBuffers.vertexBuffer,
                vertexBufferSize);

            staging.CopyToBuffer(
                commandBuffer.GetHandle(),
                *meshBuffers.indexBuffer,
                indexBufferSize,
                vertexBufferSize);
        });

    // vmaDestroyBuffer(
    //     VkContext::GetAllocator(),
    //     staging.buffer,
    //     staging.allocation);

    staging.Destroy();

    return meshBuffers;
}

void Renderer::Draw()
{
    BeginFrame();

    transitionImageLayout(
        CurrentFrame().mainCommandBuffer.GetHandle(),
        drawImage.GetHandle(),
        drawImage.GetFormat(),
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_GENERAL);

    transitionImageLayout(
        CurrentFrame().mainCommandBuffer.GetHandle(),
        depthImage.GetHandle(),
        depthImage.GetFormat(),
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
        CurrentFrame().mainCommandBuffer.GetHandle(),
        drawImage.GetHandle(),
        drawImage.GetFormat(),
        VK_IMAGE_LAYOUT_GENERAL,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    transitionImageLayout(
        CurrentFrame().mainCommandBuffer.GetHandle(),
        m_swapchain
            ->GetImage(), // m_swapchain->m_images[m_swapchain->GetImageIndex()],
        m_swapchain->GetFormat(),
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    blitImage(
        CurrentFrame().mainCommandBuffer.GetHandle(),
        drawImage.GetHandle(),
        m_swapchain
            ->GetImage(), // m_swapchain->m_images[CurrentSwapchainImageIndex()],
        drawExtent,
        m_swapchain->GetExtent());

    // transitionImageLayout(
    //     m_renderer.CurrentFrame().mainCommandBuffer,
    //     VkContext.GetSwapchain()
    //         .images[m_renderer.CurrentSwapchainImageIndex()],
    //     VkContext.GetSwapchain().imageFormat,
    //     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    //     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    //
    // VkRenderingAttachmentInfo colorAttachment{
    //     createColorAttachmentInfo(
    //         VkContext.GetSwapchain()
    //             .imageViews[m_renderer.CurrentSwapchainImageIndex()],
    //         VK_IMAGE_LAYOUT_GENERAL),
    // };

    // uiManager.Draw(
    //     m_renderer.CurrentFrame().mainCommandBuffer,
    //     colorAttachment,
    //     VkContext.GetSwapchain().extent);

    transitionImageLayout(
        CurrentFrame().mainCommandBuffer.GetHandle(),
        m_swapchain
            ->GetImage(), // m_swapchain->m_images[m_swapchain->GetImageIndex()],
        m_swapchain->GetFormat(),
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    // m_swapchain->SetLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    EndFrame();
}

void Renderer::ResizeDrawObjects(const VkExtent2D& extent)
{
    vkDeviceWaitIdle(VkContext::GetLogicalDevice());

    VkExtent2D e{ m_window.GetWidth(), m_window.GetHeight() };
    // ResizeSwapchain(e);

    // m_swapchainBuilder.setOldSwapchain(m_swapchain->GetHandle());
    // m_swapchainBuilder.setDesiredExtent(extent.width, extent.height);
    //
    // const auto& result{ m_swapchainBuilder.build() };

    // destroySwapchain(VkContext::GetDevice(), m_swapchain);
    // m_swapchain = result.value();

    m_swapchain->Resize(extent.width, extent.height);
    // LOG_DEBUG("Swapchain resized: {}x{}", extent.width, extent.height);

    // destroyImage(
    //     VkContext::GetLogicalDevice(),
    //     VkContext::GetAllocator(),
    //     drawImage);
    //
    // destroyImage(
    //     VkContext::GetLogicalDevice(),
    //     VkContext::GetAllocator(),
    //     depthImage);

    InitDrawObjects(extent);

    m_swapchainRebuildRequired = false;
    // m_window.resized = false;
}

void Renderer::InitCommands()
{
    for (std::size_t i{ 0 }; i < m_swapchain->GetFramesCount(); ++i)
    {
        std::string commandPoolName{ "CommandPool Frame " + std::to_string(i) };
        m_frames[i].commandPool = CommandPool(
            VkContext::GetDevice().GetQueueFamily(QueueType::Graphics),
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            commandPoolName.c_str());

        std::string commandBufferName{ "CommandBuffer Frame " +
                                       std::to_string(i) };
        m_frames[i].mainCommandBuffer =
            CommandBuffer(m_frames[i].commandPool, commandBufferName.c_str());
    }
}

void Renderer::InitDrawObjects(const VkExtent2D& extent)
{
    VkExtent3D drawImageExtent = {
        .width = extent.width,
        .height = extent.height,
        .depth = 1,
    };

    drawImage = Image(
        ImageType::Texture2D,
        drawImageExtent,
        VK_FORMAT_R16G16B16A16_SFLOAT,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT |
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        "Image Color Draw");

    // drawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
    // drawImage.imageExtent = drawImageExtent;

    // create2DImage(
    //     VkContext::GetAllocator(),
    //     drawImage,
    //     VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT |
    //         VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    //     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // create2DImageView(
    //     VkContext::GetLogicalDevice(),
    //     drawImage,
    //     VK_IMAGE_ASPECT_COLOR_BIT);

    depthImage = Image(
        ImageType::Texture2D,
        drawImageExtent,
        VK_FORMAT_D32_SFLOAT,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        "Image Depth Draw");

    // depthImage.imageFormat = VK_FORMAT_D32_SFLOAT;
    // depthImage.imageExtent = drawImageExtent;
    //
    // create2DImage(
    //     VkContext::GetAllocator(),
    //     depthImage,
    //     VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
    //     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // create2DImageView(
    //     VkContext::GetLogicalDevice(),
    //     depthImage,
    //     VK_IMAGE_ASPECT_DEPTH_BIT);

    // #ifndef NDEBUG
    //     setDebugUtilsObjectNameEXT(
    //         VkContext::GetLogicalDevice(),
    //         VK_OBJECT_TYPE_IMAGE,
    //         reinterpret_cast<std::uint64_t>(drawImage.image),
    //         "Image Draw Color");
    //
    //     setDebugUtilsObjectNameEXT(
    //         VkContext::GetLogicalDevice(),
    //         VK_OBJECT_TYPE_IMAGE,
    //         reinterpret_cast<std::uint64_t>(depthImage.image),
    //         "Image Draw Depth");
    //
    //     setDebugUtilsObjectNameEXT(
    //         VkContext::GetLogicalDevice(),
    //         VK_OBJECT_TYPE_IMAGE_VIEW,
    //         reinterpret_cast<std::uint64_t>(drawImage.imageView),
    //         "Image View Draw Color");
    //
    //     setDebugUtilsObjectNameEXT(
    //         VkContext::GetLogicalDevice(),
    //         VK_OBJECT_TYPE_IMAGE_VIEW,
    //         reinterpret_cast<std::uint64_t>(depthImage.imageView),
    //         "Image View Draw Depth");
    // #endif
}

void Renderer::InitDescriptors()
{
    // DescriptorLayoutBuilder descriptorLayoutBuilder;
    //
    // descriptorLayoutBuilder.AddBinding(
    //     0,
    //     VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    //     VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
    //
    // m_sceneDataDescriptorSetLayout =
    //     descriptorLayoutBuilder.Build(VkContext::GetLogicalDevice());

    sceneDataDescriptorSetLayout = DescriptorSetLayout(
        { Descriptor(
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0) },
        "DesciptorSetLayout Scene");
}

void Renderer::DrawCompute()
{
    auto& cmd{ CurrentFrame().mainCommandBuffer };

    vkCmdBindPipeline(
        cmd.GetHandle(),
        VK_PIPELINE_BIND_POINT_COMPUTE,
        computeEffect.pipeline->GetHandle());

    vkCmdBindDescriptorSets(
        cmd.GetHandle(),
        VK_PIPELINE_BIND_POINT_COMPUTE,
        computeEffect.pipeline->GetLayout(),
        0,
        1,
        &_drawImageDescriptors,
        0,
        nullptr);

    cmd.PushConstants(
        computeEffect.pipeline->GetLayout(),
        VK_SHADER_STAGE_COMPUTE_BIT,
        0,
        computeEffect.data);

    // 16x16 workgroup size so we need to divide by it
    cmd.Dispatch(
        static_cast<std::uint32_t>(std::ceil(drawExtent.width / 16.0)),
        static_cast<std::uint32_t>(std::ceil(drawExtent.height / 16.0)),
        1);
}

void Renderer::InitCompute()
{
    const std::vector<VkDescriptorPoolSize> poolSizes{
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 10 }
    };

    computeDescriptorAllocator.Init(10, poolSizes);

    drawImageDescriptorLayout = DescriptorSetLayout(
        {
            Descriptor(
                VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                VK_SHADER_STAGE_COMPUTE_BIT,
                0),
        },
        "DesciptorSetLayout Compute");

    _drawImageDescriptors = computeDescriptorAllocator.Allocate(
        drawImageDescriptorLayout.GetHandle());

    {
        DescriptorWriter descriptorWriter;
        descriptorWriter.writeDescriptorImage(
            VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            0,
            drawImage.GetView(),
            VK_NULL_HANDLE,
            VK_IMAGE_LAYOUT_GENERAL);

        descriptorWriter.updateDescriptorSet(
            VkContext::GetLogicalDevice(),
            _drawImageDescriptors);
    }

    std::vector<Shader> shaders{};

    shaders.emplace_back(Shader(
        "../engine/shaders/gradient_color.comp.spv",
        VK_SHADER_STAGE_COMPUTE_BIT));

    VkPushConstantRange pushConstant;
    pushConstant.offset = 0;
    pushConstant.size = sizeof(ComputePushConstants);
    pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    computeEffect.data = {};
    computeEffect.data.data1 = Vector4f(1, 0, 0, 1);
    computeEffect.data.data2 = Vector4f(0, 0, 1, 1);

    PipelineState state(
        std::move(shaders),
        RasterizationState::Default,
        DepthStencilState::Enabled,
        BlendState::AlphaBlending,
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

    std::vector<DescriptorSetLayout> layout;

    computeEffect.pipeline = std::make_unique<Pipeline>(Pipeline(
        state,
        { &drawImageDescriptorLayout },
        {
            PushConstants(
                VK_SHADER_STAGE_COMPUTE_BIT,
                0,
                sizeof(ComputePushConstants)),
        },
        "Pipeline Compute"));

    // vkDestroyShaderModule(
    //     VkContext::GetLogicalDevice(),
    //     computeShader,
    //     allocationCallbacks.get());
}

void Renderer::InitMesh()
{
    testMeshes =
        loadGltfMeshes(this, "D:\\Code\\Zeus\\assets\\basicmesh.glb").value();

    std::vector<Shader> shaders{};
    shaders.emplace_back(
        Shader("../engine/shaders/basic.vert.spv", VK_SHADER_STAGE_VERTEX_BIT));
    shaders.emplace_back(Shader(
        "../engine/shaders/basic.frag.spv",
        VK_SHADER_STAGE_FRAGMENT_BIT));

    PipelineState state(
        std::move(shaders), //{ vertShader, fragShader },
        RasterizationState::Default,
        DepthStencilState::Enabled,
        BlendState::AlphaBlending,
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        { drawImage.GetFormat() },
        depthImage.GetFormat());

    _meshPipeline = std::make_unique<Pipeline>(Pipeline(
        state,
        {},
        { PushConstants(
            VK_SHADER_STAGE_VERTEX_BIT,
            0,
            sizeof(MeshPushConstants)) },
        "Pipeline Mesh"));

    // vkDestroyShaderModule(
    //     VkContext::GetLogicalDevice(),
    //     vertShader,
    //     allocationCallbacks.get());
    //
    // vkDestroyShaderModule(
    //     VkContext::GetLogicalDevice(),
    //     fragShader,
    //     allocationCallbacks.get());

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

void Renderer::DrawTriangle()
{
    auto& cmd{ CurrentFrame().mainCommandBuffer };
    // InputManager::handleKeyboard();
    // InputManager::camera->Update();
    // transitionImageLayout(
    //     m_renderer.CurrentFrame().mainCommandBuffer,
    //     m_renderer.drawImage.image,
    //     m_renderer.drawImage.imageFormat,
    //     VK_IMAGE_LAYOUT_GENERAL,
    //     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    VkRenderingAttachmentInfo colorAttachmentInfo =
        createColorAttachmentInfo(drawImage.GetView(), VK_IMAGE_LAYOUT_GENERAL);

    VkRenderingAttachmentInfo depthAttachmentInfo = createDepthAttachmentInfo(
        depthImage.GetView(),
        VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

    cmdBeginRendering(
        cmd.GetHandle(),
        drawExtent,
        1,
        &colorAttachmentInfo,
        &depthAttachmentInfo);

    vkCmdBindPipeline(
        cmd.GetHandle(),
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        _meshPipeline->GetHandle());

    cmd.SetViewport({
        .x = 0.f,
        .y = 0.f,
        .width = static_cast<float>(drawExtent.width),
        .height = static_cast<float>(drawExtent.height),
        .minDepth = 0.f,
        .maxDepth = 1.f,
    });

    cmd.SetScissor({
        .offset = { .x = 0, .y = 0 },
        .extent = drawExtent,
    });

    MeshPushConstants push_constants;
    push_constants.modelMatrix = identity<float>();
    // InputManager::camera->GetViewProjection();
    // push_constants.vertexBufferAddress =
    //     testMeshes[2]->meshBuffers.vertexBufferAddress;

    push_constants.vertexBufferAddress =
        testMeshes[2]->meshBuffers.vertexBuffer->GetDeviceAddress();

    cmd.PushConstants(
        _meshPipeline->GetLayout(),
        VK_SHADER_STAGE_VERTEX_BIT,
        0,
        push_constants);

    vkCmdBindIndexBuffer(
        cmd.GetHandle(),
        testMeshes[2]->meshBuffers.indexBuffer->GetHandle(),
        0,
        VK_INDEX_TYPE_UINT32);

    cmd.DrawIndexed(
        testMeshes[2]->surfaces[0].count,
        testMeshes[2]->surfaces[0].startIndex);

    cmd.EndRendering();
}
}
