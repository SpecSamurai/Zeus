#include "Renderer.hpp"

#include "gltf/GltfLoader.hpp"
#include "input/InputManager.hpp"
#include "logging/logger.hpp"
#include "vulkan/DescriptorLayoutBuilder.hpp"
#include "vulkan/DescriptorWriter.hpp"
#include "vulkan/PipelineBuilder.hpp"
#include "vulkan/SwapchainBuilder.hpp"
#include "vulkan/VulkanContext.hpp"
#include "vulkan/vulkan_command.hpp"
#include "vulkan/vulkan_debug.hpp"
#include "vulkan/vulkan_dynamic_rendering.hpp"
#include "vulkan/vulkan_image.hpp"
#include "vulkan/vulkan_pipeline.hpp"
#include "vulkan/vulkan_shader.hpp"
#include "vulkan/vulkan_sync.hpp"
#include "window/Window.hpp"

#include <vulkan/vulkan_core.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace Zeus
{
Renderer::Renderer(Window& window, VulkanContext& vkContext)
    : m_window{ window },
      m_vkContext{ vkContext }
{
}

void Renderer::Init()
{
    InitSwapchain();
    InitSyncObjects();
    InitCommands();
    InitDrawObjects(m_window.extent);
    InitDescriptors();

    InitCompute();
    InitMesh();
}

void Renderer::Destroy()
{
    LOG_DEBUG("Destroying Renderer");
    destroyImage(
        m_vkContext.device.logicalDevice,
        m_vkContext.allocator,
        drawImage);

    destroyImage(
        m_vkContext.device.logicalDevice,
        m_vkContext.allocator,
        depthImage);

    vkDestroyCommandPool(
        m_vkContext.device.logicalDevice,
        m_ImmediateSubmitCommandPool,
        allocationCallbacks.get());

    for (std::size_t i{ 0 }; i < m_swapchain.maxConcurrentFrames; ++i)
    {
        vkDestroyCommandPool(
            m_vkContext.device.logicalDevice,
            m_frames[i].commandPool,
            allocationCallbacks.get());
    }

    vkDestroyFence(
        m_vkContext.device.logicalDevice,
        m_ImmediateSubmitFence,
        allocationCallbacks.get());

    for (std::size_t i{ 0 }; i < m_swapchain.maxConcurrentFrames; ++i)
    {
        vkDestroyFence(
            m_vkContext.device.logicalDevice,
            m_frames[i].renderFence,
            allocationCallbacks.get());

        vkDestroySemaphore(
            m_vkContext.device.logicalDevice,
            m_frames[i].imageAcquiredSemaphore,
            allocationCallbacks.get());

        vkDestroySemaphore(
            m_vkContext.device.logicalDevice,
            m_frames[i].renderCompleteSemaphore,
            allocationCallbacks.get());
    }

    destroySwapchain(m_vkContext.device, m_swapchain);
}

VkResult Renderer::BeginFrame()
{
    VKCHECK(
        vkWaitForFences(
            m_vkContext.device.logicalDevice,
            1,
            &CurrentFrame().renderFence,
            VK_TRUE,
            UINT64_MAX),
        "Failed to wait for fence");

    // getCurrentFrame().deletionQueue.flush();
    // getCurrentFrame().descriptorAllocator.Clear(vkContext.device.logicalDevice);

    VkResult result{ vkAcquireNextImageKHR(
        m_vkContext.device.logicalDevice,
        m_swapchain.handle,
        UINT64_MAX,
        CurrentFrame().imageAcquiredSemaphore,
        VK_NULL_HANDLE,
        &m_swapchainImageIndex) };

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        m_swapchainRebuildRequired = true;
        return result;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        LOG_ERROR("Failed to acquire swapchain image");
        return result;
    }

    drawExtent = {
        .width = static_cast<std::uint32_t>(
            static_cast<float>(drawImage.imageExtent.width) * m_renderScale),
        .height = static_cast<std::uint32_t>(
            static_cast<float>(drawImage.imageExtent.height) * m_renderScale),
    };

    VKCHECK(
        vkResetFences(
            m_vkContext.device.logicalDevice,
            1,
            &CurrentFrame().renderFence),
        "Failed to reset fence");

    VKCHECK(
        vkResetCommandBuffer(CurrentFrame().mainCommandBuffer, 0),
        "Failed to reset command buffer");

    return beginVkCommandBuffer(
        CurrentFrame().mainCommandBuffer,
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
}

VkResult Renderer::EndFrame()
{
    VKCHECK(
        vkEndCommandBuffer(CurrentFrame().mainCommandBuffer),
        "Failed to record command buffer");

    VkCommandBufferSubmitInfo submitInfo{ createVkCommandBufferSubmitInfo(
        CurrentFrame().mainCommandBuffer) };

    VkSemaphoreSubmitInfo waitSemaphoreInfo{ createVkSemaphoreSubmitInfo(
        CurrentFrame().imageAcquiredSemaphore,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT) };

    VkSemaphoreSubmitInfo signalSemaphoreInfo{ createVkSemaphoreSubmitInfo(
        CurrentFrame().renderCompleteSemaphore,
        VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT) };

    cmdVkQueueSubmit2(
        m_vkContext.device.graphicsQueue,
        CurrentFrame().renderFence,
        1,
        &waitSemaphoreInfo,
        1,
        &submitInfo,
        1,
        &signalSemaphoreInfo);

    VkResult presentResult{ cmdVkQueuePresentKHR(
        m_vkContext.device.presentQueue,
        1,
        &CurrentFrame().renderCompleteSemaphore,
        1,
        &m_swapchain.handle,
        &m_swapchainImageIndex) };

    if (presentResult == VK_ERROR_OUT_OF_DATE_KHR ||
        presentResult == VK_SUBOPTIMAL_KHR)
    {
        m_swapchainRebuildRequired = true;
    }
    else if (presentResult != VK_SUCCESS)
    {
        LOG_ERROR("Failed to present swapchain image");
        return presentResult;
    }

    m_currentFrame = (m_currentFrame + 1) % m_swapchain.maxConcurrentFrames;

    return presentResult;
}

MeshBuffers Renderer::UploadMesh(
    std::span<Vertex> vertices,
    std::span<std::uint32_t> indices)
{
    const VkDeviceSize vertexBufferSize{ vertices.size() * sizeof(Vertex) };
    const VkDeviceSize indexBufferSize{ indices.size() *
                                        sizeof(std::uint32_t) };

    MeshBuffers meshBuffers{};

    createBuffer(
        m_vkContext.allocator,
        meshBuffers.vertexBuffer,
        vertexBufferSize,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    meshBuffers.vertexBufferAddress = getBufferDeviceAddress(
        m_vkContext.device.logicalDevice,
        meshBuffers.vertexBuffer.buffer);

    createBuffer(
        m_vkContext.allocator,
        meshBuffers.indexBuffer,
        indexBufferSize,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    Buffer staging{};
    createBuffer(
        m_vkContext.allocator,
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

    CmdImmediateSubmit([&](VkCommandBuffer commandBuffer) {
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

    vmaDestroyBuffer(m_vkContext.allocator, staging.buffer, staging.allocation);

    return meshBuffers;
}

void Renderer::Draw()
{
    BeginFrame();

    transitionImageLayout(
        CurrentFrame().mainCommandBuffer,
        drawImage.image,
        drawImage.imageFormat,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_GENERAL);

    transitionImageLayout(
        CurrentFrame().mainCommandBuffer,
        depthImage.image,
        depthImage.imageFormat,
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
        CurrentFrame().mainCommandBuffer,
        drawImage.image,
        drawImage.imageFormat,
        VK_IMAGE_LAYOUT_GENERAL,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    transitionImageLayout(
        CurrentFrame().mainCommandBuffer,
        GetSwapchain().images[CurrentSwapchainImageIndex()],
        GetSwapchain().imageFormat,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    blitImage(
        CurrentFrame().mainCommandBuffer,
        drawImage.image,
        GetSwapchain().images[CurrentSwapchainImageIndex()],
        drawExtent,
        GetSwapchain().extent);

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
        CurrentFrame().mainCommandBuffer,
        GetSwapchain().images[CurrentSwapchainImageIndex()],
        GetSwapchain().imageFormat,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    EndFrame();
}

void Renderer::ResizeSwapchain(const VkExtent2D& extent)
{
    m_swapchainBuilder.setOldSwapchain(m_swapchain.handle);
    m_swapchainBuilder.setDesiredExtent(extent.width, extent.height);

    const auto& result{ m_swapchainBuilder.build() };

    destroySwapchain(m_vkContext.device, m_swapchain);
    m_swapchain = result.value();

    LOG_DEBUG("Swapchain resized: {}x{}", extent.width, extent.height);
}

void Renderer::CmdImmediateSubmit(
    std::function<void(VkCommandBuffer cmd)>&& function)
{
    VKCHECK(
        vkResetFences(
            m_vkContext.device.logicalDevice,
            1,
            &m_ImmediateSubmitFence),
        "Failed to reset fence");

    VKCHECK(
        vkResetCommandBuffer(m_ImmediateSubmitCommandBuffer, 0),
        "Failed to reset command buffer");

    beginVkCommandBuffer(
        m_ImmediateSubmitCommandBuffer,
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    function(m_ImmediateSubmitCommandBuffer);

    VKCHECK(
        vkEndCommandBuffer(m_ImmediateSubmitCommandBuffer),
        "Failed to end command buffer");

    VkCommandBufferSubmitInfo submitInfo{ createVkCommandBufferSubmitInfo(
        m_ImmediateSubmitCommandBuffer) };

    cmdVkQueueSubmit2(
        m_vkContext.device.transferQueue,
        m_ImmediateSubmitFence,
        0,
        nullptr,
        1,
        &submitInfo,
        0,
        nullptr);

    VKCHECK(
        vkWaitForFences(
            m_vkContext.device.logicalDevice,
            1,
            &m_ImmediateSubmitFence,
            VK_TRUE,
            UINT64_MAX),
        "Failed to wait for fence");
}

void Renderer::ResizeDrawObjects(const VkExtent2D& extent)
{
    vkDeviceWaitIdle(m_vkContext.device.logicalDevice);

    ResizeSwapchain(m_window.extent);

    destroyImage(
        m_vkContext.device.logicalDevice,
        m_vkContext.allocator,
        drawImage);

    destroyImage(
        m_vkContext.device.logicalDevice,
        m_vkContext.allocator,
        depthImage);

    InitDrawObjects(extent);

    m_swapchainRebuildRequired = false;
    m_window.resized = false;
}

void Renderer::InitSwapchain()
{
    m_swapchainBuilder = SwapchainBuilder{};
    m_swapchainBuilder.setDevice(m_vkContext.device, m_vkContext.surface);
    m_swapchainBuilder.setDesiredExtent(
        m_window.extent.width,
        m_window.extent.height);
    m_swapchainBuilder.setDesiredPresentMode(VK_PRESENT_MODE_MAILBOX_KHR);
    m_swapchainBuilder.setImageUsageFlags(
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);

    m_swapchain = m_swapchainBuilder.build().value();
    m_frames.resize(m_swapchain.maxConcurrentFrames);
}

void Renderer::InitSyncObjects()
{
    VKCHECK(
        createVkFence(
            m_vkContext.device.logicalDevice,
            true,
            m_ImmediateSubmitFence),
        "Failed to create oneTimeSubmit fence.");

#ifndef NDEBUG
    setDebugUtilsObjectNameEXT(
        m_vkContext.device.logicalDevice,
        VK_OBJECT_TYPE_FENCE,
        reinterpret_cast<std::uint64_t>(m_ImmediateSubmitFence),
        "Fence One-Time Submit");
#endif

    for (std::size_t i{ 0 }; i < m_swapchain.maxConcurrentFrames; ++i)
    {
        VKCHECK(
            createVkFence(
                m_vkContext.device.logicalDevice,
                true,
                m_frames[i].renderFence),
            "Failed to create frame fence.");

        VKCHECK(
            createVkSemaphore(
                m_vkContext.device.logicalDevice,
                m_frames[i].imageAcquiredSemaphore),
            "Failed to create frame semaphore.");

        VKCHECK(
            createVkSemaphore(
                m_vkContext.device.logicalDevice,
                m_frames[i].renderCompleteSemaphore),
            "Failed to create frame semaphore.");

#ifndef NDEBUG
        std::string renderFenceName{ "Fence Frame " + std::to_string(i) };
        setDebugUtilsObjectNameEXT(
            m_vkContext.device.logicalDevice,
            VK_OBJECT_TYPE_FENCE,
            reinterpret_cast<std::uint64_t>(m_frames[i].renderFence),
            renderFenceName.c_str());

        std::string imageSemaphoreName{ "Semaphore Frame Image " +
                                        std::to_string(i) };
        setDebugUtilsObjectNameEXT(
            m_vkContext.device.logicalDevice,
            VK_OBJECT_TYPE_SEMAPHORE,
            reinterpret_cast<std::uint64_t>(m_frames[i].imageAcquiredSemaphore),
            imageSemaphoreName.c_str());

        std::string renderSemaphoreName{ "Semaphore Frame Render " +
                                         std::to_string(i) };
        setDebugUtilsObjectNameEXT(
            m_vkContext.device.logicalDevice,
            VK_OBJECT_TYPE_SEMAPHORE,
            reinterpret_cast<std::uint64_t>(
                m_frames[i].renderCompleteSemaphore),
            renderSemaphoreName.c_str());
#endif
    }
}

void Renderer::InitCommands()
{
    createVkCommandPool(
        m_vkContext.device.logicalDevice,
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        m_vkContext.device.transferFamily,
        m_ImmediateSubmitCommandPool);

    allocateVkCommandBuffer(
        m_ImmediateSubmitCommandBuffer,
        m_vkContext.device.logicalDevice,
        m_ImmediateSubmitCommandPool);

#ifndef NDEBUG
    setDebugUtilsObjectNameEXT(
        m_vkContext.device.logicalDevice,
        VK_OBJECT_TYPE_COMMAND_POOL,
        reinterpret_cast<std::uint64_t>(m_ImmediateSubmitCommandPool),
        "CommandPool One-Time");

    setDebugUtilsObjectNameEXT(
        m_vkContext.device.logicalDevice,
        VK_OBJECT_TYPE_COMMAND_BUFFER,
        reinterpret_cast<std::uint64_t>(m_ImmediateSubmitCommandBuffer),
        "CommandBuffer One-Time");
#endif

    for (std::size_t i{ 0 }; i < m_swapchain.maxConcurrentFrames; ++i)
    {
        createVkCommandPool(
            m_vkContext.device.logicalDevice,
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            m_vkContext.device.graphicsFamily,
            m_frames[i].commandPool);

        allocateVkCommandBuffer(
            m_frames[i].mainCommandBuffer,
            m_vkContext.device.logicalDevice,
            m_frames[i].commandPool);

#ifndef NDEBUG
        std::string commandPoolName{ "CommandPool Frame " + std::to_string(i) };
        setDebugUtilsObjectNameEXT(
            m_vkContext.device.logicalDevice,
            VK_OBJECT_TYPE_COMMAND_POOL,
            reinterpret_cast<std::uint64_t>(m_frames[i].commandPool),
            commandPoolName.c_str());

        std::string commandBufferName{ "CommandBuffer Frame " +
                                       std::to_string(i) };
        setDebugUtilsObjectNameEXT(
            m_vkContext.device.logicalDevice,
            VK_OBJECT_TYPE_COMMAND_BUFFER,
            reinterpret_cast<std::uint64_t>(m_frames[i].mainCommandBuffer),
            commandBufferName.c_str());
#endif
    }
}

void Renderer::InitDrawObjects(const VkExtent2D& extent)
{
    VkExtent3D drawImageExtent = {
        .width = extent.width,
        .height = extent.height,
        .depth = 1,
    };

    drawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
    drawImage.imageExtent = drawImageExtent;

    create2DImage(
        m_vkContext.allocator,
        drawImage,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT |
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    create2DImageView(
        m_vkContext.device.logicalDevice,
        drawImage,
        VK_IMAGE_ASPECT_COLOR_BIT);

    depthImage.imageFormat = VK_FORMAT_D32_SFLOAT;
    depthImage.imageExtent = drawImageExtent;

    create2DImage(
        m_vkContext.allocator,
        depthImage,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    create2DImageView(
        m_vkContext.device.logicalDevice,
        depthImage,
        VK_IMAGE_ASPECT_DEPTH_BIT);

#ifndef NDEBUG
    setDebugUtilsObjectNameEXT(
        m_vkContext.device.logicalDevice,
        VK_OBJECT_TYPE_IMAGE,
        reinterpret_cast<std::uint64_t>(drawImage.image),
        "Image Draw Color");

    setDebugUtilsObjectNameEXT(
        m_vkContext.device.logicalDevice,
        VK_OBJECT_TYPE_IMAGE,
        reinterpret_cast<std::uint64_t>(depthImage.image),
        "Image Draw Depth");

    setDebugUtilsObjectNameEXT(
        m_vkContext.device.logicalDevice,
        VK_OBJECT_TYPE_IMAGE_VIEW,
        reinterpret_cast<std::uint64_t>(drawImage.imageView),
        "Image View Draw Color");

    setDebugUtilsObjectNameEXT(
        m_vkContext.device.logicalDevice,
        VK_OBJECT_TYPE_IMAGE_VIEW,
        reinterpret_cast<std::uint64_t>(depthImage.imageView),
        "Image View Draw Depth");
#endif
}

void Renderer::InitDescriptors()
{
    DescriptorLayoutBuilder descriptorLayoutBuilder;

    descriptorLayoutBuilder.AddBinding(
        0,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

    m_sceneDataDescriptorSetLayout =
        descriptorLayoutBuilder.Build(m_vkContext.device.logicalDevice);
}

void Renderer::DrawCompute()
{
    vkCmdBindPipeline(
        CurrentFrame().mainCommandBuffer,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        computeEffect.pipeline);

    vkCmdBindDescriptorSets(
        CurrentFrame().mainCommandBuffer,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        computeEffect.layout,
        0,
        1,
        &_drawImageDescriptors,
        0,
        nullptr);

    vkCmdPushConstants(
        CurrentFrame().mainCommandBuffer,
        computeEffect.layout,
        VK_SHADER_STAGE_COMPUTE_BIT,
        0,
        sizeof(ComputePushConstants),
        &computeEffect.data);

    // 16x16 workgroup size so we need to divide by it
    vkCmdDispatch(
        CurrentFrame().mainCommandBuffer,
        std::ceil(drawExtent.width / 16.0),
        std::ceil(drawExtent.height / 16.0),
        1);
}

void Renderer::InitCompute()
{
    const std::vector<VkDescriptorPoolSize> poolSizes{
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 10 }
    };

    computeDescriptorAllocator.init(
        m_vkContext.device.logicalDevice,
        10,
        poolSizes);

    {
        DescriptorLayoutBuilder descriptorLayoutBuilder;
        descriptorLayoutBuilder.AddBinding(
            0,
            VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            VK_SHADER_STAGE_COMPUTE_BIT);
        _drawImageDescriptorLayout =
            descriptorLayoutBuilder.Build(m_vkContext.device.logicalDevice);
    }

    _drawImageDescriptors = computeDescriptorAllocator.allocate(
        m_vkContext.device.logicalDevice,
        _drawImageDescriptorLayout);

    {
        DescriptorWriter descriptorWriter;
        descriptorWriter.writeDescriptorImage(
            VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            0,
            drawImage.imageView,
            VK_NULL_HANDLE,
            VK_IMAGE_LAYOUT_GENERAL);

        descriptorWriter.updateDescriptorSet(
            m_vkContext.device.logicalDevice,
            _drawImageDescriptors);
    }

    VkShaderModule computeShader;
    loadShader(
        computeShader,
        m_vkContext.device.logicalDevice,
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
        m_vkContext.device.logicalDevice,
        1,
        &_drawImageDescriptorLayout,
        1,
        &pushConstant);

    createComputePipeline(
        computeEffect.pipeline,
        m_vkContext.device.logicalDevice,
        computeEffect.layout,
        computeShader);

    vkDestroyShaderModule(
        m_vkContext.device.logicalDevice,
        computeShader,
        allocationCallbacks.get());
}

void Renderer::InitMesh()
{
    testMeshes = loadGltfMeshes(this, "D:\\Code\\Zeus\\basicmesh.glb").value();

    VkShaderModule vertShader;
    loadShader(
        vertShader,
        m_vkContext.device.logicalDevice,
        "../engine/shaders/basic.vert.spv");

    VkShaderModule fragShader;
    loadShader(
        fragShader,
        m_vkContext.device.logicalDevice,
        "../engine/shaders/basic.frag.spv");

    VkPushConstantRange pushConstantRange{};
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(MeshPushConstants);
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    createVkPipelineLayout(
        meshPipelineLayout,
        m_vkContext.device.logicalDevice,
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
        builder.enableDepthTest(true, VK_COMPARE_OP_GREATER_OR_EQUAL);
        builder.disableMiltisampling();
        builder.setDynamicStates(
            { VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_VIEWPORT });

        builder.dynamicRendering();
        builder.setColorAttachmentFormat(drawImage.imageFormat);
        builder.setDepthFormat(depthImage.imageFormat);

        meshPipeline =
            builder.build(m_vkContext.device.logicalDevice, meshPipelineLayout);
    }

    vkDestroyShaderModule(
        m_vkContext.device.logicalDevice,
        vertShader,
        allocationCallbacks.get());

    vkDestroyShaderModule(
        m_vkContext.device.logicalDevice,
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

void Renderer::DrawTriangle()
{
    InputManager::handleKeyboard();
    InputManager::camera->Update();
    // transitionImageLayout(
    //     m_renderer.CurrentFrame().mainCommandBuffer,
    //     m_renderer.drawImage.image,
    //     m_renderer.drawImage.imageFormat,
    //     VK_IMAGE_LAYOUT_GENERAL,
    //     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    VkRenderingAttachmentInfo colorAttachmentInfo =
        createColorAttachmentInfo(drawImage.imageView, VK_IMAGE_LAYOUT_GENERAL);

    VkRenderingAttachmentInfo depthAttachmentInfo = createDepthAttachmentInfo(
        depthImage.imageView,
        VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

    cmdBeginRendering(
        CurrentFrame().mainCommandBuffer,
        drawExtent,
        1,
        &colorAttachmentInfo,
        &depthAttachmentInfo);

    vkCmdBindPipeline(
        CurrentFrame().mainCommandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        meshPipeline);

    cmdSetVkViewport(
        CurrentFrame().mainCommandBuffer,
        drawExtent.width,
        drawExtent.height);

    cmdSetVkScissor(CurrentFrame().mainCommandBuffer, drawExtent);

    MeshPushConstants push_constants;
    push_constants.modelMatrix = InputManager::camera->GetViewProjection();
    push_constants.vertexBufferAddress =
        testMeshes[2]->meshBuffers.vertexBufferAddress;

    vkCmdPushConstants(
        CurrentFrame().mainCommandBuffer,
        meshPipelineLayout,
        VK_SHADER_STAGE_VERTEX_BIT,
        0,
        sizeof(MeshPushConstants),
        &push_constants);

    vkCmdBindIndexBuffer(
        CurrentFrame().mainCommandBuffer,
        testMeshes[2]->meshBuffers.indexBuffer.buffer,
        0,
        VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(
        CurrentFrame().mainCommandBuffer,
        testMeshes[2]->surfaces[0].count,
        1,
        testMeshes[2]->surfaces[0].startIndex,
        0,
        0);

    vkCmdEndRendering(CurrentFrame().mainCommandBuffer);
}
}
