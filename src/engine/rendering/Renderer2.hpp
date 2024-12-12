#pragma once

#include "core/defines.hpp"
#include "logging/logger.hpp"
#include "rendering/resources.hpp"
#include "rhi/CommandBuffer.hpp"
#include "rhi/CommandPool.hpp"
#include "rhi/Swapchain.hpp"
#include "rhi/VkContext.hpp"
#include "rhi/vulkan/vulkan_dynamic_rendering.hpp"

#include <vulkan/vulkan_core.h>

#include <array>
#include <cstdint>
#include <format>
#include <vector>

namespace Zeus
{
enum class RenderTargets
{
    RENDER_OUTPUT_COLOR,
    RENDER_OUTPUT_DEPTH,
    COUNT
};

class Renderer2
{
public:
    Renderer2(const Window& window) : m_window{ window }
    {
    }

    void Initialize()
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
        // InitializeDefaultResources();
    }

    void Destroy()
    {
        LOG_DEBUG("Destroying Renderer");
        // temp
        VkContext::GetDevice().Wait();

        // DestroyDefaultResources();

        m_basicPipeline->Destroy();

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

    void Draw()
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

        auto& cmd{ CurrentFrame().graphicsCommandBuffer };
        cmd.Reset();
        cmd.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        cmd.TransitionImageLayout(
            m_renderTargets[0].GetHandle(),
            m_renderTargets[0].GetFormat(),
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_GENERAL);

        cmd.TransitionImageLayout(
            m_renderTargets[1].GetHandle(),
            m_renderTargets[1].GetFormat(),
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

        vkCmdBindPipeline(
            cmd.GetHandle(),
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_basicPipeline->GetHandle());

        VkRenderingAttachmentInfo colorAttachmentInfo =
            createColorAttachmentInfo(
                m_renderTargets[0].GetView(),
                VK_IMAGE_LAYOUT_GENERAL);

        VkRenderingAttachmentInfo depthAttachmentInfo =
            createDepthAttachmentInfo(
                m_renderTargets[1].GetView(),
                VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

        cmd.BeginRendering(
            { m_renderTargets[0].GetExtent().width,
              m_renderTargets[0].GetExtent().height },
            1,
            &colorAttachmentInfo,
            &depthAttachmentInfo);

        cmd.SetViewport({
            .x = 0.f,
            .y = 0.f,
            .width = static_cast<float>(m_renderTargets[0].GetExtent().width),
            .height = static_cast<float>(m_renderTargets[0].GetExtent().height),
            .minDepth = 0.f,
            .maxDepth = 1.f,
        });

        cmd.SetScissor({
            .offset = { .x = 0, .y = 0 },
            .extent = { .width = m_renderTargets[0].GetExtent().width,
                        .height = m_renderTargets[0].GetExtent().height },
        });

        cmd.Draw(3, 0);

        cmd.EndRendering();

        cmd.TransitionImageLayout(
            m_renderTargets[0].GetHandle(),
            m_renderTargets[0].GetFormat(),
            VK_IMAGE_LAYOUT_GENERAL,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

        m_swapchain.SetLayout(cmd, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        cmd.BlitImage(m_renderTargets[0], m_swapchain);

        m_swapchain.SetLayout(cmd, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
        cmd.End();

        m_swapchain.Present(cmd.GetHandle());
    }

private:
    struct Frame
    {
        CommandPool graphicsCommandPool;
        CommandBuffer graphicsCommandBuffer;

        // Per Frame allocation
        // DescriptorAllocator descriptorAllocator;
        // TODO: conisider adding draw resources per frame
        // Image Backbuffer;
    };

    inline constexpr Frame& CurrentFrame()
    {
        return m_frames[m_swapchain.GetFrameIndex()];
    }

    void ResizeSwapchain()
    {
        VkContext::GetDevice().Wait();
        m_swapchain.Resize(m_window.GetWidth(), m_window.GetHeight());

        // drawImage.Destroy();
        // depthImage.Destroy();
        //
        // InitDrawObjects({ m_window.GetWidth(), m_window.GetHeight() });
    }

    void InitializeRenderTargets()
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
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
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

    void InitializeShaders()
    {
        m_shaders[0] = Shader(
            std::format("{}/basic.vert.spv", ShadersFolderPath).data(),
            VK_SHADER_STAGE_VERTEX_BIT,
            "main",
            {},
            "Shader_basic_vert");

        m_shaders[1] = Shader(
            std::format("{}/basic.frag.spv", ShadersFolderPath).data(),
            VK_SHADER_STAGE_FRAGMENT_BIT,
            "main",
            {},
            "Shader_basic_frag");
    }

    void InitializePipelines()
    {
        std::vector<Shader> shaders;
        shaders.emplace_back(Shader(
            std::format("{}/basic.vert.spv", ShadersFolderPath).data(),
            VK_SHADER_STAGE_VERTEX_BIT,
            "main",
            {},
            "Shader_basic_vert"));

        shaders.emplace_back(Shader(
            std::format("{}/basic.frag.spv", ShadersFolderPath).data(),
            VK_SHADER_STAGE_FRAGMENT_BIT,
            "main",
            {},
            "Shader_basic_frag"));

        PipelineState pipelineState(
            std::move(shaders),
            Rasterization::Default,
            DepthStencil::DefaultDisabled,
            Blending::Disabled,
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            { VK_FORMAT_R16G16B16A16_SFLOAT },
            VK_FORMAT_D32_SFLOAT);

        m_basicPipeline = new Pipeline(pipelineState, {}, {}, "Pipeline_basic");
    }

public:
    static constexpr std::int32_t FRAMES_IN_FLIGHT{ 2 };

private:
    const Window& m_window;
    Swapchain m_swapchain;
    std::array<Frame, FRAMES_IN_FLIGHT> m_frames;

    // turn it into per frame resoruce
    std::array<Image, static_cast<std::uint32_t>(RenderTargets::COUNT)>
        m_renderTargets;

    std::array<Shader, 2> m_shaders;
    Pipeline* m_basicPipeline;
};
}
