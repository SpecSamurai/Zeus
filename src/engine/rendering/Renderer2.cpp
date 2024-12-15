#include "Renderer2.hpp"

#include "logging/logger.hpp"
#include "math/definitions.hpp"
#include "resources.hpp"
#include "rhi/Buffer.hpp"
#include "rhi/CommandBuffer.hpp"
#include "rhi/CommandPool.hpp"
#include "rhi/Swapchain.hpp"
#include "rhi/Vertex.hpp"
#include "rhi/VkContext.hpp"
#include "rhi/vulkan/vulkan_dynamic_rendering.hpp"
#include "window/Window.hpp"

#include <vulkan/vulkan_core.h>

#include <array>
#include <cstdint>
#include <format>
#include <vector>

namespace Zeus
{
Renderer2::Renderer2(const Window& window) : m_window{ window }
{
}

void Renderer2::Initialize()
{
    LOG_DEBUG("Initializing Renderer");

    m_swapchain = Swapchain(
        VkContext::GetSurface(),
        m_window.GetWidth(),
        m_window.GetHeight(),
        VK_PRESENT_MODE_MAILBOX_KHR,
        FRAMES_IN_FLIGHT);

    for (std::uint32_t i{ 0 }; i < m_swapchain.GetFramesCount(); ++i)
    {
        m_frames[i].graphicsCommandPool = CommandPool(
            VkContext::GetQueueFamily(QueueType::Graphics),
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            std::format("CommandPool_frame_{}", i));

        m_frames[i].graphicsCommandBuffer = CommandBuffer(
            m_frames[i].graphicsCommandPool,
            std::format("CommandBuffer_frame_{}", i));
    }

    InitializeRenderTargets();
    InitializeShaders();
    InitializePipelines();
    InitializeBuffers();
    // InitializeDefaultResources();

    DrawTriangle(
        Vector3f(0.0f, -0.5f, 0.8f),
        Vector3f(0.5, 0.5, 0),
        Vector3f(-0.5, 0.5, 0),
        Colors::GREEN);

    DrawRectangle(
        Vector3f(0.0f, 0.0f, 0.8f),
        Vector3f(0.5, 0.0, 0),
        Vector3f(0.5, 0.5, 0),
        Vector3f(0, 0.5, 0),
        Colors::BLUE);

    m_linesVertexBuffer.Update(
        m_lines.data(),
        { m_lines.size() * sizeof(Vertex_PositionColor) });
}

void Renderer2::Destroy()
{
    LOG_DEBUG("Destroying Renderer");
    // temp
    VkContext::GetDevice().Wait();

    m_linesVertexBuffer.Destroy();

    // DestroyDefaultResources();

    for (std::uint32_t i{ 0 }; i < m_pipelines.size(); ++i)
    {
        m_pipelines[i]->Destroy();
    }

    for (std::uint32_t i{ 0 }; i < m_shaders.size(); ++i)
    {
        m_shaders[i].Destroy();
    }

    for (std::uint32_t i{ 0 }; i < m_renderTargets.size(); ++i)
    {
        m_renderTargets[i].Destroy();
    }

    for (std::uint32_t i{ 0 }; i < m_frames.size(); ++i)
    {
        m_frames[i].graphicsCommandPool.Destroy();
    }

    m_swapchain.Destroy();
}

void Renderer2::Update()
{
    if (m_swapchain.IsResizeRequired())
    {
        ResizeSwapchain();
    }

    m_swapchain.AcquireNextImage();

    // drawExtent = {
    //     .width = static_cast<std::uint32_t>(
    //         static_cast<float>(drawImage.GetWidth()) * m_renderScale),
    //     .height = static_cast<std::uint32_t>(
    //         static_cast<float>(drawImage.GetHeight()) * m_renderScale),
    // };

    auto& renderOutputColor{ GetRenderTarget(
        RenderTargets::RENDER_OUTPUT_COLOR) };
    auto& renderOutputDepth{ GetRenderTarget(
        RenderTargets::RENDER_OUTPUT_DEPTH) };

    auto& cmd{ CurrentFrame().graphicsCommandBuffer };
    cmd.Reset();
    cmd.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    cmd.TransitionImageLayout(
        renderOutputColor.GetHandle(),
        renderOutputColor.GetFormat(),
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    cmd.TransitionImageLayout(
        renderOutputDepth.GetHandle(),
        renderOutputDepth.GetFormat(),
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

    vkCmdBindPipeline(
        cmd.GetHandle(),
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        GetPipeline(PipelineTypes::LINES)->GetHandle());

    cmd.BindVertexBuffers(m_linesVertexBuffer);

    VkRenderingAttachmentInfo colorAttachmentInfo = createColorAttachmentInfo(
        renderOutputColor.GetView(),
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    VkRenderingAttachmentInfo depthAttachmentInfo = createDepthAttachmentInfo(
        renderOutputDepth.GetView(),
        VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

    cmd.BeginRendering(
        { renderOutputColor.GetExtent().width,
          renderOutputColor.GetExtent().height },
        1,
        &colorAttachmentInfo,
        &depthAttachmentInfo);

    cmd.SetViewport({
        .x = 0.f,
        .y = 0.f,
        .width = static_cast<float>(renderOutputColor.GetExtent().width),
        .height = static_cast<float>(renderOutputColor.GetExtent().height),
        .minDepth = 0.f,
        .maxDepth = 1.f,
    });

    cmd.SetScissor({
        .offset = { .x = 0, .y = 0 },
        .extent = { .width = renderOutputColor.GetExtent().width,
                    .height = renderOutputColor.GetExtent().height },
    });

    cmd.Draw((std::uint32_t)m_lines.size(), 0);

    cmd.EndRendering();

    cmd.TransitionImageLayout(
        renderOutputColor.GetHandle(),
        renderOutputColor.GetFormat(),
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    m_swapchain.SetLayout(cmd, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    cmd.BlitImage(renderOutputColor, m_swapchain);

    m_swapchain.SetLayout(cmd, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    cmd.End();

    m_swapchain.Present(cmd.GetHandle());
}

void Renderer2::DrawLine(
    const Vector3f& from,
    const Vector3f& to,
    const Color& fromColor,
    const Color& toColor)
{
    m_lines[m_linesIndex++] = Vertex_PositionColor{
        .position = from,
        .color = fromColor,
    };

    m_lines[m_linesIndex++] = Vertex_PositionColor{
        .position = to,
        .color = toColor,
    };
}

void Renderer2::DrawTriangle(
    const Vector3f& vertex1,
    const Vector3f& vertex2,
    const Vector3f& vertex3,
    const Color& color)
{
    DrawLine(vertex1, vertex2, color, color);
    DrawLine(vertex2, vertex3, color, color);
    DrawLine(vertex3, vertex1, color, color);
}

void Renderer2::DrawRectangle(
    const Vector3f& topLeft,
    const Vector3f& topRight,
    const Vector3f& bottomRight,
    const Vector3f& bottomLeft,
    const Color& color)
{
    DrawLine(topLeft, topRight, color, color);
    DrawLine(topRight, bottomRight, color, color);
    DrawLine(bottomRight, bottomLeft, color, color);
    DrawLine(bottomLeft, topLeft, color, color);
}

const Image& Renderer2::GetRenderTarget(RenderTargets type) const
{
    return m_renderTargets[static_cast<std::uint32_t>(type)];
}

const Shader& Renderer2::GetShader(ShaderModuleTypes type) const
{
    return m_shaders[static_cast<std::uint32_t>(type)];
}

const Pipeline* Renderer2::GetPipeline(PipelineTypes type) const
{
    return m_pipelines[static_cast<std::uint32_t>(type)];
}

void Renderer2::ResizeSwapchain()
{
    VkContext::GetDevice().Wait();
    m_swapchain.Resize(m_window.GetWidth(), m_window.GetHeight());

    // drawImage.Destroy();
    // depthImage.Destroy();
    //
    // InitDrawObjects({ m_window.GetWidth(), m_window.GetHeight() });
}

void Renderer2::InitializeRenderTargets()
{
#define renderTarget(type) m_renderTargets[static_cast<std::uint32_t>(type)]

    VkExtent3D renderOutputExtent{
        .width = m_window.GetWidth(),
        .height = m_window.GetHeight(),
        .depth = 1,
    };

    renderTarget(RenderTargets::RENDER_OUTPUT_COLOR) = Image(
        ImageType::Texture2D,
        renderOutputExtent,
        VK_FORMAT_R16G16B16A16_SFLOAT,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
            VK_IMAGE_USAGE_STORAGE_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        "Image_render_output_color_frame_");

    renderTarget(RenderTargets::RENDER_OUTPUT_DEPTH) = Image(
        ImageType::Texture2D,
        renderOutputExtent,
        VK_FORMAT_D32_SFLOAT,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        "Image_render_output_depth_frame_");
}

void Renderer2::InitializeShaders()
{
#define shader(type) m_shaders[static_cast<std::uint32_t>(type)]
    shader(ShaderModuleTypes::FLAT_COLOR_FRAG) = Shader(
        std::format("{}/flat_color.frag.spv", ShadersFolderPath).data(),
        VK_SHADER_STAGE_FRAGMENT_BIT,
        "main",
        {},
        "Shader_fragment_flat_color");

    shader(ShaderModuleTypes::LINE_VERT) = Shader(
        std::format("{}/line.vert.spv", ShadersFolderPath).data(),
        VK_SHADER_STAGE_VERTEX_BIT,
        "main",
        {
            VertexInput(
                0,
                sizeof(Vertex_PositionColor),
                VK_VERTEX_INPUT_RATE_VERTEX,
                {
                    VertexInputAttribute(
                        0,
                        0,
                        offsetof(Vertex_PositionColor, position),
                        VK_FORMAT_R32G32B32_SFLOAT),
                    VertexInputAttribute(
                        1,
                        0,
                        offsetof(Vertex_PositionColor, color),
                        VK_FORMAT_R32G32B32A32_SFLOAT),
                }),
        },
        "Shader_vertex_line");
}

void Renderer2::InitializePipelines()
{
    PipelineState pipelineState(
        Rasterization::Default,
        DepthStencil::DefaultDisabled,
        Blending::Disabled,
        VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
        { VK_FORMAT_R16G16B16A16_SFLOAT },
        VK_FORMAT_D32_SFLOAT);

    m_pipelines[static_cast<std::uint32_t>(PipelineTypes::LINES)] =
        new Pipeline(
            pipelineState,
            {
                &GetShader(ShaderModuleTypes::LINE_VERT),
                &GetShader(ShaderModuleTypes::FLAT_COLOR_FRAG),
            },
            {},
            {},
            "Pipeline_basic");
}

void Renderer2::InitializeBuffers()
{
    m_linesVertexBuffer = Buffer(
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        m_lines.size() * sizeof(Vertex_PositionColor),
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        false,
        "Buffer_vertex_lines");
}
}
