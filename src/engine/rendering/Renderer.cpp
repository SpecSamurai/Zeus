#include "Renderer.hpp"

#include "Renderer_resources.hpp"
#include "Renderer_types.hpp"
#include "components/Renderable.hpp"
#include "logging/logger.hpp"
#include "math/definitions.hpp"
#include "rhi/Buffer.hpp"
#include "rhi/CommandBuffer.hpp"
#include "rhi/CommandPool.hpp"
#include "rhi/Definitions.hpp"
#include "rhi/Descriptor.hpp"
#include "rhi/DescriptorSet.hpp"
#include "rhi/PushConstants.hpp"
#include "rhi/Swapchain.hpp"
#include "rhi/Vertex.hpp"
#include "rhi/VkContext.hpp"
#include "rhi/vulkan/vulkan_dynamic_rendering.hpp"
#include "window/Window.hpp"

#include <vulkan/vulkan_core.h>

#include <array>
#include <cstdint>
#include <format>
#include <memory>
#include <string_view>
#include <vector>

namespace Zeus
{
Renderer::Renderer(const Window& window) : m_window{ window }
{
    m_lines.reserve(LINES_BUFFER_BASE_SIZE);
}

void Renderer::Initialize()
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
            std::format("CommandPool_Frame_{}", i));

        m_frames[i].graphicsCommandBuffer = CommandBuffer(
            m_frames[i].graphicsCommandPool,
            std::format("CommandBuffer_Frame_{}", i));
    }

    InitializeRenderTargets();
    InitializeDescriptors();
    InitializeShaders();
    InitializePipelines();
    InitializeSamplers();
    InitializeBuffers();

    InitializeDefaultResources();
}

void Renderer::Destroy()
{
    LOG_DEBUG("Destroying Renderer");

    VkContext::GetDevice().Wait();

    DestroyDefaultResources();

    m_frameDataBuffer.Destroy();
    m_linesVertexBuffer->Destroy();

    for (std::uint32_t i{ 0 }; i < m_samplers.size(); ++i)
    {
        m_samplers[i].Destroy();
    }

    for (std::uint32_t i{ 0 }; i < m_pipelines.size(); ++i)
    {
        m_pipelines[i].Destroy();
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

    m_frameDataSetLayout.Destroy();
    m_descriptorPool.Destroy();
    m_swapchain.Destroy();
}

void Renderer::Update()
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
        RenderTarget::RENDER_OUTPUT_COLOR) };
    auto& renderOutputDepth{ GetRenderTarget(
        RenderTarget::RENDER_OUTPUT_DEPTH) };

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

    // cmdClearColorImage(
    //     m_renderer.CurrentFrame().mainCommandBuffer,
    //     m_renderer.drawImage.image,
    //     Vector4f(0.f, 0.f, 0.5f, 1.f),
    //     VK_IMAGE_LAYOUT_GENERAL);

    // DrawCompute();

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

    DrawEntities(cmd, renderOutputColor);
    LinesPass(cmd, renderOutputColor);

    // DrawCompute();
    // DrawTriangle();

    cmd.EndRendering();

    cmd.TransitionImageLayout(
        renderOutputColor.GetHandle(),
        renderOutputColor.GetFormat(),
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    m_swapchain.SetLayout(cmd, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    cmd.BlitImage(renderOutputColor, m_swapchain);

    //  drawExtent,
    //  m_swapchain->GetExtent());

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

    m_swapchain.SetLayout(cmd, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    cmd.End();

    m_swapchain.Present(cmd.GetHandle());
}

void Renderer::DrawLine(
    const Math::Vector3f& from,
    const Math::Vector3f& to,
    const Math::Color& fromColor,
    const Math::Color& toColor)
{
    m_lines.emplace_back(Vertex_PositionColor{
        .position = from,
        .color = fromColor,
    });

    m_lines.emplace_back(Vertex_PositionColor{
        .position = to,
        .color = toColor,
    });
}

void Renderer::DrawTriangle(
    const Math::Vector3f& vertex1,
    const Math::Vector3f& vertex2,
    const Math::Vector3f& vertex3,
    const Math::Color& color)
{
    DrawLine(vertex1, vertex2, color, color);
    DrawLine(vertex2, vertex3, color, color);
    DrawLine(vertex3, vertex1, color, color);
}

void Renderer::DrawRectangle(
    const Math::Vector3f& topLeft,
    const Math::Vector3f& topRight,
    const Math::Vector3f& bottomRight,
    const Math::Vector3f& bottomLeft,
    const Math::Color& color)
{
    DrawLine(topLeft, topRight, color, color);
    DrawLine(topRight, bottomRight, color, color);
    DrawLine(bottomRight, bottomLeft, color, color);
    DrawLine(bottomLeft, topLeft, color, color);
}

const Image& Renderer::GetRenderTarget(RenderTarget type) const
{
    return m_renderTargets[static_cast<std::uint32_t>(type)];
}

const Shader& Renderer::GetShader(ShaderType type) const
{
    return m_shaders[static_cast<std::uint32_t>(type)];
}

const Pipeline& Renderer::GetPipeline(PipelineType type) const
{
    return m_pipelines[static_cast<std::uint32_t>(type)];
}

const Sampler& Renderer::GetSampler(SamplerType type) const
{
    return m_samplers[static_cast<std::uint32_t>(type)];
}

void Renderer::SetEntities(
    RendererEntity type,
    const std::vector<Renderable>& renderables)
{
    // mutex lock
    GetEntities(type).clear();

    for (const auto& entity : renderables)
    {
        // isActive
        GetEntities(type).emplace_back(entity);
    }

    if (type == RendererEntity::MESH_TRANSPARENT)
    {
        /*std::sort(*/
        /*    m_renderables[type].begin(),*/
        /*    m_renderables[type].end(),*/
        /*    [&](const auto& a, const auto& b) { return true; });*/
    }

    // mutex lock
}

void Renderer::SetCameraProjection(const Math::Matrix4x4f& viewProjection)
{
    m_frameDataBuffer.Update(&viewProjection, sizeof(Math::Matrix4x4f));
}

void Renderer::ResizeSwapchain()
{
    VkContext::GetDevice().Wait();
    m_swapchain.Resize(m_window.GetWidth(), m_window.GetHeight());

    // drawImage.Destroy();
    // depthImage.Destroy();
    //
    // InitDrawObjects({ m_window.GetWidth(), m_window.GetHeight() });
}

void Renderer::InitializeRenderTargets()
{
#define renderTarget(type) m_renderTargets[static_cast<std::uint32_t>(type)]

    VkExtent3D renderOutputExtent{
        .width = m_window.GetWidth(),
        .height = m_window.GetHeight(),
        .depth = 1,
    };

    renderTarget(RenderTarget::RENDER_OUTPUT_COLOR) = Image(
        ImageType::Texture2D,
        renderOutputExtent,
        VK_FORMAT_R16G16B16A16_SFLOAT,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
            VK_IMAGE_USAGE_STORAGE_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        "Image_Render_Output_Color_Frame_");

    renderTarget(RenderTarget::RENDER_OUTPUT_DEPTH) = Image(
        ImageType::Texture2D,
        renderOutputExtent,
        VK_FORMAT_D32_SFLOAT,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        "Image_Render_Output_Depth_Frame_");
}

void Renderer::InitializeDescriptors()
{
    std::vector<VkDescriptorPoolSize> poolSizes{
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 },
    };

    m_descriptorPool = DescriptorPool(1, poolSizes);

    m_frameDataSetLayout = DescriptorSetLayout({
        Descriptor(
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            VK_SHADER_STAGE_VERTEX_BIT,
            0),
    });
}

void Renderer::InitializeShaders()
{
    constexpr std::string_view SHADERS_FOLDER_PATH{ "../engine/shaders" };

#define shader(type) m_shaders[static_cast<std::uint32_t>(type)]

    shader(ShaderType::FLAT_COLOR_FRAG) = Shader(
        "Shader_Fragment_flat_color",
        std::format("{}/flat_color.frag.spv", SHADERS_FOLDER_PATH).data(),
        VK_SHADER_STAGE_FRAGMENT_BIT,
        "main");

    shader(ShaderType::LINE_VERT) = Shader(
        "Shader_Vertex_line",
        std::format("{}/line.vert.spv", SHADERS_FOLDER_PATH).data(),
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
        });

    shader(ShaderType::MESH) = Shader(
        "Shader_Vertex_Mesh",
        std::format("{}/mesh.vert.spv", SHADERS_FOLDER_PATH).data(),
        VK_SHADER_STAGE_VERTEX_BIT,
        "main");

    shader(ShaderType::FRAG_MESH) = Shader(
        "Shader_Fragment_Mesh",
        std::format("{}/mesh.frag.spv", SHADERS_FOLDER_PATH).data(),
        VK_SHADER_STAGE_FRAGMENT_BIT,
        "main");
}

void Renderer::InitializePipelines()
{
#define pipeline(type) m_pipelines[static_cast<std::uint32_t>(type)]

    pipeline(PipelineType::LINES) = Pipeline(
        "Pipeline_Lines",
        Rasterization::Default,
        DepthStencil::DefaultDisabled,
        Blending::Disabled,
        VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
        {
            &GetShader(ShaderType::LINE_VERT),
            &GetShader(ShaderType::FLAT_COLOR_FRAG),
        },
        { &m_frameDataSetLayout },
        {},
        { VK_FORMAT_R16G16B16A16_SFLOAT },
        VK_FORMAT_D32_SFLOAT);

    PushConstants meshPushConstant(
        VK_SHADER_STAGE_VERTEX_BIT,
        0,
        sizeof(MeshPushConstants));

    pipeline(PipelineType::MESH_OPAQUE) = Pipeline(
        "Pipeline_Mesh_Opaque",
        Rasterization::Default,
        DepthStencil::DefaultEnabled,
        Blending::Disabled,
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        {
            &GetShader(ShaderType::MESH),
            &GetShader(ShaderType::FRAG_MESH),
        },
        { &m_frameDataSetLayout },
        { meshPushConstant },
        { VK_FORMAT_R16G16B16A16_SFLOAT },
        VK_FORMAT_D32_SFLOAT);
}

void Renderer::InitializeSamplers()
{
#define sampler(type) m_samplers[static_cast<std::uint32_t>(type)]

    sampler(SamplerType::NEAREST_CLAMP_EDGE) = Sampler(
        VK_FILTER_NEAREST,
        VK_FILTER_NEAREST,
        VK_SAMPLER_MIPMAP_MODE_NEAREST,
        VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);

    sampler(SamplerType::LINEAR_CLAMP_EDGE) = Sampler(
        VK_FILTER_LINEAR,
        VK_FILTER_LINEAR,
        VK_SAMPLER_MIPMAP_MODE_NEAREST,
        VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
}

void Renderer::InitializeBuffers()
{
    m_linesVertexBuffer = std::make_shared<Buffer>(Buffer(
        "Buffer_vertex_lines",
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        m_lines.capacity() * sizeof(Vertex_PositionColor),
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        false));

    m_frameDataBuffer = Buffer(
        "Buffer_frame_data",
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        sizeof(FrameData),
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        true);

    m_frameDataSet = DescriptorSet(m_frameDataSetLayout, m_descriptorPool);
    m_frameDataSet.Update(
        {
            DescriptorBuffer(
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                VK_SHADER_STAGE_VERTEX_BIT,
                0,
                m_frameDataBuffer.GetHandle(),
                sizeof(FrameData)),
        },
        {});
}

void Renderer::DrawEntities(const CommandBuffer& cmd, const Image& renderTarget)
{
    const auto meshes{ GetEntities(RendererEntity::MESH_OPAQUE) };
    if (meshes.empty())
        return;

    vkCmdBindPipeline(
        cmd.GetHandle(),
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        GetPipeline(PipelineType::MESH_OPAQUE).GetHandle());

    cmd.SetViewport({
        .x = 0.f,
        .y = 0.f,
        .width = static_cast<float>(renderTarget.GetExtent().width),
        .height = static_cast<float>(renderTarget.GetExtent().height),
        .minDepth = 0.f,
        .maxDepth = 1.f,
    });

    cmd.SetScissor({
        .offset = { .x = 0, .y = 0 },
        .extent = { .width = renderTarget.GetExtent().width,
                    .height = renderTarget.GetExtent().height },
    });

    cmd.BindDescriptorSets(
        m_frameDataSet.GetHandle(),
        GetPipeline(PipelineType::MESH_OPAQUE));

    for (const auto& entity : meshes)
    {
        MeshPushConstants pushConstants{
            .model = entity.localMatrix,
            .vertexBufferAddress =
                entity.m_mesh->GetVertexBuffer()->GetDeviceAddress(),
        };

        cmd.PushConstants(
            GetPipeline(PipelineType::MESH_OPAQUE).GetLayout(),
            VK_SHADER_STAGE_VERTEX_BIT,
            0,
            pushConstants);

        /*cmd.BindVertexBuffers(mesh.m_mesh->m_vertexBuffer);*/
        cmd.BindIndexBuffer(*entity.m_mesh->GetIndexBuffer());

        /*cmd.Draw((std::uint32_t)mesh.m_mesh->m_vertexBuffer.GetSize(), 0);*/
        /*cmd.DrawIndexed(static_cast<std::uint32_t>(entity.m_mesh->m_indices.size()));*/
        cmd.DrawIndexed(entity.m_mesh->GetIndexCount());
    }
}

void Renderer::LinesPass(const CommandBuffer& cmd, const Image& renderTarget)
{
    if (!m_lines.empty())
    {
        if (m_lines.size() * sizeof(Vertex_PositionColor) >
            m_linesVertexBuffer->GetSize())
        {
            m_linesVertexBuffer = std::make_shared<Buffer>(Buffer(
                "Buffer_vertex_lines",
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                    VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                m_lines.capacity() * sizeof(Vertex_PositionColor),
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                false));
        }

        m_linesVertexBuffer->Update(
            m_lines.data(),
            m_lines.size() * sizeof(Vertex_PositionColor));

        vkCmdBindPipeline(
            cmd.GetHandle(),
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            GetPipeline(PipelineType::LINES).GetHandle());

        cmd.BindDescriptorSets(
            m_frameDataSet.GetHandle(),
            GetPipeline(PipelineType::LINES));

        cmd.BindVertexBuffers(*m_linesVertexBuffer);

        cmd.SetViewport({
            .x = 0.f,
            .y = 0.f,
            .width = static_cast<float>(renderTarget.GetExtent().width),
            .height = static_cast<float>(renderTarget.GetExtent().height),
            .minDepth = 0.f,
            .maxDepth = 1.f,
        });

        cmd.SetScissor({
            .offset = { .x = 0, .y = 0 },
            .extent = { .width = renderTarget.GetExtent().width,
                        .height = renderTarget.GetExtent().height },
        });

        cmd.Draw(static_cast<std::uint32_t>(m_lines.size()), 0);

        m_lines.clear();
    }
}

std::vector<Renderable>& Renderer::GetEntities(RendererEntity entity)
{
    return m_renderables[static_cast<std::uint32_t>(entity)];
}
}
