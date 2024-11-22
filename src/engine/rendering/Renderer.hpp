#pragma once

#include "Renderer_definitions.hpp"
#include "renderer_types.hpp"
#include "vulkan/DescriptorPool.hpp"
#include "vulkan/DescriptorSet.hpp"
#include "vulkan/DescriptorSetLayout.hpp"
#include "vulkan/Image.hpp"
#include "vulkan/Pipeline.hpp"
#include "vulkan/Swapchain.hpp"
#include "window/Window.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <memory>
#include <span>

namespace Zeus
{
class Renderer
{
public:
    Renderer(Window& window);

    void Init();
    void Destroy();

    void BeginFrame();
    void EndFrame();

    MeshBuffers UploadMesh(
        std::span<Vertex> vertices,
        std::span<std::uint32_t> indices);

    void Draw();

private:
    void InitCommands();
    void InitDrawObjects(std::uint32_t width, std::uint32_t height);
    void ResizeDrawObjects();
    void InitDescriptors();

    void InitCompute();
    void DrawCompute();

    void InitMesh();
    void DrawTriangle();

    inline constexpr FrameData& CurrentFrame()
    {
        return m_frames[m_swapchain->GetFrameIndex()];
    }

private:
    Window& m_window;
    std::unique_ptr<Swapchain> m_swapchain;

    std::vector<FrameData> m_frames;

    // DrawContext m_drawContext{};
    // SceneData m_sceneData{};

    DescriptorSetLayout sceneDataDescriptorSetLayout;

    float m_renderScale{ 1.f };

    VkExtent2D drawExtent;
    Image drawImage;
    Image depthImage;

    // COMPUTE ******************
    DescriptorPool computeDescriptorAllocator;
    DescriptorSet _drawImageDescriptors;
    DescriptorSetLayout drawImageDescriptorLayout;

    struct ComputePushConstants
    {
        Vector4f data1;
        Vector4f data2;
        Vector4f data3;
        Vector4f data4;
    };

    struct ComputeEffect
    {
        const char* name;
        std::unique_ptr<Pipeline> pipeline;
        ComputePushConstants data;
    } computeEffect;
    // COMPUTE ******************

    // TRAINGLE/MESH
    MeshBuffers rectangle;
    std::unique_ptr<Pipeline> _meshPipeline;

    std::vector<std::shared_ptr<MeshAsset>> testMeshes;
    // TRAINGLE/MESH

    float renderScale{ 1.f };

    // bool stopRendering{ false };
};
}
