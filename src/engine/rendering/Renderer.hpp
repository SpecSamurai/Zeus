#pragma once

#include "Renderer_types.hpp"
#include "components/Renderable.hpp"
#include "math/definitions.hpp"
#include "rhi/Buffer.hpp"
#include "rhi/CommandBuffer.hpp"
#include "rhi/CommandPool.hpp"
#include "rhi/DescriptorSet.hpp"
#include "rhi/Swapchain.hpp"
#include "rhi/Vertex.hpp"
#include "window/Window.hpp"

#include <memory>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan_core.h>

#include <array>
#include <cstdint>

namespace Zeus
{
class Renderer
{
public:
    Renderer(const Window& window);

    void Initialize();
    void Destroy();
    void Update();

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

    const Image& GetRenderTarget(RenderTargets type) const;
    const Shader& GetShader(ShaderModuleTypes type) const;
    const Pipeline* GetPipeline(PipelineTypes type) const;

    void SetEntities(
        RendererEntity type,
        const std::vector<Renderable>& renderables);

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
    void InitializeBuffers();

    void DrawEntities(const CommandBuffer& cmd, const Image& renderTarget);
    void LinesPass(const CommandBuffer& cmd, const Image& renderTarget);

public:
    static constexpr std::int32_t FRAMES_IN_FLIGHT{ 2 };
    static constexpr std::uint32_t LINES_BUFFER_BASE_SIZE{ 32768 };

public:
    const Window& m_window;
    Swapchain m_swapchain;
    std::array<Frame, FRAMES_IN_FLIGHT> m_frames;

    DescriptorPool m_descriptorPool;

    // turn it into per frame resoruce
    std::array<Image, static_cast<std::uint32_t>(RenderTargets::COUNT)>
        m_renderTargets;
    std::array<Shader, static_cast<std::uint32_t>(ShaderModuleTypes::COUNT)>
        m_shaders;
    std::array<Pipeline*, static_cast<std::uint32_t>(PipelineTypes::COUNT)>
        m_pipelines;

    std::shared_ptr<Buffer> m_linesVertexBuffer;
    std::uint64_t m_linesIndex{ 0 };
    std::vector<Vertex_PositionColor> m_lines;

    std::unordered_map<RendererEntity, std::vector<Renderable>> m_renderables;

    FrameData m_frameData;
    DescriptorSetLayout m_frameDataSetLayout;
    DescriptorSet m_frameDataSet;
    Buffer m_frameDataBuffer;
};
}
