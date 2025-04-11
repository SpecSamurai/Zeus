#pragma once

#include "Renderer_types.hpp"
#include "components/Renderable.hpp"
#include "ecs/Query.hpp"
#include "math/definitions.hpp"
#include "rhi/Buffer.hpp"
#include "rhi/CommandBuffer.hpp"
#include "rhi/CommandPool.hpp"
#include "rhi/DescriptorSet.hpp"
#include "rhi/Sampler.hpp"
#include "rhi/Swapchain.hpp"
#include "rhi/Vertex.hpp"
#include "window/Window.hpp"

#include <vulkan/vulkan_core.h>

#include <array>
#include <cstdint>
#include <memory>
#include <vector>

namespace Zeus
{
class Renderer
{
public:
    Renderer(const Window& window);

    void Initialize();
    void Destroy();
    void Update();

    void BeginFrame();
    void Draw();
    void BlitToSwapchain();
    void Present();

    void DrawLine(
        const Math::Vector3f& from,
        const Math::Vector3f& to,
        const Math::Color& fromColor,
        const Math::Color& toColor);

    void DrawTriangle(
        const Math::Vector3f& vertex1,
        const Math::Vector3f& vertex2,
        const Math::Vector3f& vertex3,
        const Math::Color& color);

    void DrawRectangle(
        const Math::Vector3f& topLeft,
        const Math::Vector3f& topRight,
        const Math::Vector3f& bottomRight,
        const Math::Vector3f& bottomLeft,
        const Math::Color& color);

    const Image& GetRenderTarget(RenderTarget type) const;
    const Shader& GetShader(ShaderType type) const;
    const Pipeline& GetPipeline(PipelineType type) const;
    const Sampler& GetSampler(SamplerType type) const;

    inline constexpr Swapchain& GetSwapchain()
    {
        return m_swapchain;
    }

    inline constexpr CommandBuffer& GetCommandBuffer()
    {
        return CurrentFrame().graphicsCommandBuffer;
    }

    void SetEntities(RendererEntity type, ECS::Query<Renderable>& query);

    void SetCameraProjection(const Math::Matrix4x4f& viewProjection);

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

    void ResizeSwapchain();

    void InitializeRenderTargets();
    void InitializeDescriptors();
    void InitializeShaders();
    void InitializePipelines();
    void InitializeSamplers();
    void InitializeBuffers();

    void DrawEntities(const CommandBuffer& cmd, const Image& renderTarget);
    void LinesPass(const CommandBuffer& cmd, const Image& renderTarget);

    std::vector<Renderable*>& GetEntities(RendererEntity entity);

private:
    static constexpr std::uint32_t FRAMES_IN_FLIGHT{ 3 };
    static constexpr std::uint32_t LINES_BUFFER_BASE_SIZE{ 32768 };

    static constexpr VkClearValue CLEAR_VALUES{};

    const Window& m_window;
    class Swapchain m_swapchain;
    std::array<Frame, FRAMES_IN_FLIGHT> m_frames;

    /*float m_renderScale{ 1.f };*/
    /*    inline void SetCamera(const Math::Matrix4x4f& viewProjection)*/
    /*{*/
    /*    m_cameraData.m_viewProjection = viewProjection;*/
    /*}*/

    // might need to set camera instead of updating scene buffer
    /*struct CameraData*/
    /*{*/
    /*    Math::Matrix4x4f m_viewProjection{ Math::identity<float>() };*/
    /*} m_cameraData;*/

    DescriptorPool m_descriptorPool;

    // turn it into per frame resoruce
    std::array<Image, static_cast<std::uint32_t>(RenderTarget::COUNT)>
        m_renderTargets;

    std::array<Shader, static_cast<std::uint32_t>(ShaderType::COUNT)> m_shaders;
    std::array<Pipeline, static_cast<std::uint32_t>(PipelineType::COUNT)>
        m_pipelines;
    std::array<Sampler, static_cast<std::uint32_t>(SamplerType::COUNT)>
        m_samplers;

    std::shared_ptr<Buffer> m_linesVertexBuffer;
    std::uint64_t m_linesIndex{ 0 };
    std::vector<Vertex_PositionColor> m_lines;

    std::array<
        std::vector<Renderable*>,
        static_cast<std::uint32_t>(RendererEntity::COUNT)>
        m_renderables;

    // Ensure resource synchronization (e.g., per-frame uniform buffers) to
    // prevent data races
    FrameData m_frameData;
    DescriptorSetLayout m_frameDataSetLayout;
    DescriptorSet m_frameDataSet;
    Buffer m_frameDataBuffer;

public:
    DescriptorSetLayout m_materialSetLayout;
    DescriptorSet m_materialSet;
};
}
