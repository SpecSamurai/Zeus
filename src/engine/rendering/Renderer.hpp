#pragma once

#include "Renderer_definitions.hpp"
#include "renderer_types.hpp"
#include "vulkan/DescriptorAllocator.hpp"
#include "vulkan/SwapchainBuilder.hpp"
#include "vulkan/VulkanContext.hpp"
#include "vulkan/vulkan_image.hpp"
#include "window/Window.hpp"

#include <functional>
#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <span>

namespace Zeus
{
class Renderer
{
public:
    Renderer(Window& window, VulkanContext& vkContext);

    void Init();
    void Destroy();

    VkResult BeginFrame();
    VkResult EndFrame();

    MeshBuffers UploadMesh(
        std::span<Vertex> vertices,
        std::span<std::uint32_t> indices);

    void Draw();

    void ResizeDrawObjects(const VkExtent2D& extent);

    inline constexpr bool ResizeRequired() const
    {
        return m_window.resized || m_swapchainRebuildRequired;
    }

    inline constexpr FrameData& CurrentFrame()
    {
        return m_frames[m_currentFrame];
    }

    inline constexpr std::uint32_t CurrentSwapchainImageIndex() const
    {
        return m_swapchainImageIndex;
    }

    inline constexpr Swapchain& GetSwapchain()
    {
        return m_swapchain;
    }

private:
    void InitSwapchain();
    void InitSyncObjects();
    void InitCommands();
    void InitDrawObjects(const VkExtent2D& extent);
    void InitDescriptors();

    void ResizeSwapchain(const VkExtent2D& extent);

    void CmdImmediateSubmit(
        std::function<void(VkCommandBuffer cmd)>&& function);

    void InitCompute();
    void DrawCompute();

    void InitMesh();
    void DrawTriangle();

private:
    Window& m_window;
    VulkanContext& m_vkContext;
    SwapchainBuilder m_swapchainBuilder;
    Swapchain m_swapchain;

    std::vector<FrameData> m_frames;

    // DrawContext m_drawContext{};
    // SceneData m_sceneData{};

    VkDescriptorSetLayout m_sceneDataDescriptorSetLayout;

    std::uint32_t m_currentFrame{ 0 };
    std::uint32_t m_swapchainImageIndex;

    float m_renderScale{ 1.f };
    bool m_swapchainRebuildRequired{ false };

private:
    VkFence m_ImmediateSubmitFence{ VK_NULL_HANDLE };
    VkCommandPool m_ImmediateSubmitCommandPool{ VK_NULL_HANDLE };
    VkCommandBuffer m_ImmediateSubmitCommandBuffer{ VK_NULL_HANDLE };

public:
    VkExtent2D drawExtent;
    Image drawImage;
    Image depthImage;

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

    std::vector<std::shared_ptr<MeshAsset>> testMeshes;
    // TRAINGLE/MESH

    float renderScale{ 1.f };

    // bool stopRendering{ false };
};
}
