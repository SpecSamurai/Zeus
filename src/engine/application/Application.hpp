#pragma once

#include "renderer/Scene.hpp"
// #include "ui/UIManager.hpp"

#include <renderer/Renderer.hpp>
#include <span>
#include <vulkan/DescriptorAllocator.hpp>
#include <vulkan/VulkanContext.hpp>
#include <window/Window.hpp>

#include <vulkan/vulkan_core.h>

#include <cstdint>

namespace Zeus
{
struct EngineStatistics
{
    float frametime;
    float fps;
    float meshDrawTime;
    float sceneUpdateTime;
    int triangleCount;
    int drawcallCount;
};

class Application
{
public:
    Application(std::uint32_t width, std::uint32_t height, const char* title);
    void Init();
    void Run();
    void Shutdown();

    MeshBuffers UploadMesh(
        std::span<Vertex> vertices,
        std::span<std::uint32_t> indices);

private:
    void Draw();

    void InitCompute();
    void DrawCompute();

    void InitMesh();
    void DrawTriangle();

private:
    Zeus::Window m_window;
    Zeus::VulkanContext m_vkContext;
    Zeus::Renderer m_renderer;

    // UIManager uiManager;

    // COMPUTE ******************
    DescriptorAllocator computeDescriptorAllocator;
    VkDescriptorSet _drawImageDescriptors;
    VkDescriptorSetLayout _drawImageDescriptorLayout;

    // VkPipeline _computePipeline;
    // VkPipelineLayout _computePipelineLayout;

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

        VkPipeline pipeline{ VK_NULL_HANDLE };
        VkPipelineLayout layout{ VK_NULL_HANDLE };

        ComputePushConstants data;
    } computeEffect;
    // COMPUTE ******************

    // TRAINGLE/MESH
    MeshBuffers rectangle;

    VkPipelineLayout meshPipelineLayout;
    VkPipeline meshPipeline;
    // TRAINGLE/MESH

    float renderScale{ 1.f };
    EngineStatistics statistics;

    // bool stopRendering{ false };
};
}
