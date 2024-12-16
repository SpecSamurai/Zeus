#pragma once

#include "Renderer_BufferData.hpp"
#include "Renderer_definitions.hpp"
#include "math/definitions.hpp"
#include "rhi/Buffer.hpp"
#include "rhi/CommandBuffer.hpp"
#include "rhi/CommandPool.hpp"
#include "rhi/DescriptorSet.hpp"
#include "rhi/Swapchain.hpp"
#include "rhi/Vertex.hpp"
#include "window/Window.hpp"

#include <vulkan/vulkan_core.h>

#include <array>
#include <cstdint>
#include <vector>

namespace Zeus
{
class Renderer2
{
public:
    Renderer2(const Window& window);

    void Initialize();
    void Destroy();
    void Update();

    void DrawLine(
        const Vector3f& from,
        const Vector3f& to,
        const Color& fromColor,
        const Color& toColor);

    void DrawTriangle(
        const Vector3f& vertex1,
        const Vector3f& vertex2,
        const Vector3f& vertex3,
        const Color& color);

    void DrawRectangle(
        const Vector3f& topLeft,
        const Vector3f& topRight,
        const Vector3f& bottomRight,
        const Vector3f& bottomLeft,
        const Color& color);

    const Image& GetRenderTarget(RenderTargets type) const;
    const Shader& GetShader(ShaderModuleTypes type) const;
    const Pipeline* GetPipeline(PipelineTypes type) const;

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

    Buffer m_linesVertexBuffer;
    std::uint64_t m_linesIndex{ 0 };
    std::array<Vertex_PositionColor, LINES_BUFFER_BASE_SIZE> m_lines;

    FrameData m_frameData;
    DescriptorSetLayout m_frameDataSetLayout;
    DescriptorSet m_frameDataSet;
    Buffer m_frameDataBuffer;
};
}
