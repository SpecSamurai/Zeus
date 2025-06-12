#pragma once

#include "Renderer_types.hpp"
#include "Vertex.hpp"
#include "components/Renderable.hpp"
#include "ecs/Query.hpp"
#include "math/definitions.hpp"
#include "rendering/Material.hpp"
#include "rhi/Buffer.hpp"
#include "rhi/CommandBuffer.hpp"
#include "rhi/CommandPool.hpp"
#include "rhi/DescriptorSet.hpp"
#include "rhi/DescriptorSetLayout.hpp"
#include "rhi/Image.hpp"
#include "rhi/Sampler.hpp"
#include "rhi/Swapchain.hpp"
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

    // Core
    void Initialize();
    void Destroy();
    void Update();

    void BeginFrame();
    void Draw();
    void BlitToSwapchain();
    void Present();

    // Primitives
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

    // Getters/Setters
    const Image& GetRenderTarget(RenderTarget type) const;
    const Shader& GetShader(ShaderType type) const;
    const Sampler& GetSampler(SamplerType type) const;
    const Pipeline& GetPipeline(PipelineType type) const;
    void SetEntities(RendererEntity type, ECS::Query<Renderable>& query);
    void SetCameraProjection(const Math::Matrix4x4f& viewProjection);

    // Bindless

    // Swapchain/Frames
    constexpr Swapchain& GetSwapchain()
    {
        return m_swapchain;
    }

    constexpr CommandBuffer& GetCommandBuffer()
    {
        return CurrentFrame().graphicsCommandBuffer;
    }

public:
    // Swapchain/Frames
    void ResizeSwapchain();

    struct RendererFrame;
    inline constexpr RendererFrame& CurrentFrame()
    {
        return m_frames[m_swapchain.GetFrameIndex()];
    }

    // Core
    void InitializeRenderTargets();
    void InitializeBuffers();
    void InitializeSamplers();
    void InitializeDescriptors();
    void InitializeShaders();
    void InitializePipelines();

    void UpdateFrameDataBuffer();

    // Passes
    void DrawEntities(const CommandBuffer& cmd, const Image& renderTarget);
    void PbrPass();
    void LinesPass(const CommandBuffer& cmd, const Image& renderTarget);

    std::vector<const Renderable*>& GetEntities(RendererEntity entity);

    struct RendererFrame
    {
        CommandPool graphicsCommandPool;
        CommandBuffer graphicsCommandBuffer;

        // Per Frame allocation
        // DescriptorAllocator descriptorAllocator;
        // TODO: conisider adding draw resources per frame
        // Image Backbuffer;
        DescriptorSet m_frameDataSet;
        Buffer m_frameDataBuffer;
    };

    struct Bindless
    {
        static constexpr std::uint32_t SET_INDEX{ 0 };

        static constexpr std::uint32_t COMBINED_IMAGE_SAMPLER_BINDING{ 0 };
        static constexpr std::uint32_t STORAGE_BUFFER_BINDING{ 1 };

        static constexpr std::uint32_t COMBINED_IMAGE_SAMPLER_COUNT{ 16384 };
        static constexpr std::uint32_t STORAGE_BUFFER_COUNT{ 16384 };

        DescriptorPool descriptorPool;
        DescriptorSetLayout descriptorSetLayout;
        DescriptorSet descriptorSet;

        std::array<Material*, COMBINED_IMAGE_SAMPLER_COUNT> materials;
        std::array<MaterialParameters, STORAGE_BUFFER_COUNT> materialParameters;

        Buffer materialParametersBuffer;
    } m_bindless;

private:
    static constexpr std::uint32_t FRAMES_IN_FLIGHT{ 3 };
    static constexpr std::uint32_t LINES_BUFFER_BASE_SIZE{ 32768 };

    static constexpr VkClearValue CLEAR_VALUES{};

    // Core
    const Window& m_window;
    class Swapchain m_swapchain;

#define array(type, count) std::array<type, static_cast<std::uint32_t>(count)>

    array(RendererFrame, FRAMES_IN_FLIGHT) m_frames;
    array(Sampler, SamplerType::COUNT) m_samplers;
    array(Shader, ShaderType::COUNT) m_shaders;
    array(Pipeline, PipelineType::COUNT) m_pipelines;

    // turn it into per frame resoruce
    array(Image, RenderTarget::COUNT) m_renderTargets;
    array(std::vector<const Renderable*>, RendererEntity::COUNT) m_renderables;

    // We use uniform buffer here instead of SSBO because this is a small
    // buffer. We arent using it through buffer device adress because we have a
    // single descriptor set for all objects so there isnt any overhead of
    // managing it.

    // Primitives
    std::shared_ptr<Buffer> m_linesVertexBuffer;
    std::uint64_t m_linesIndex{ 0 };
    std::vector<Vertex_PositionColor> m_lines;

    // Ensure resource synchronization (e.g., per-frame uniform buffers) to
    // prevent data races
    FrameData m_frameData;
    DescriptorPool m_frameDataDescriptorPool;
    DescriptorSetLayout m_frameDataSetLayout;
    DescriptorSet m_frameDataSet;
    Buffer m_frameDataBuffer;
};
}
