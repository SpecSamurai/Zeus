#pragma once

#include "ui/UIManager.hpp"

#include <renderer/Renderer.hpp>
#include <vulkan/DescriptorAllocator.hpp>
#include <vulkan/VulkanContext.hpp>
#include <window/Window.hpp>

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <functional>

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

    void cmdOneTimeSubmit(std::function<void(VkCommandBuffer cmd)>&& function);

private:
    void Draw();

    void RecreateSwapchain();
    void InitSyncObjects();
    void InitCommands();
    void InitDrawObjects();
    void InitDescriptors();

private:
    Zeus::Window m_window;
    Zeus::VulkanContext m_vkContext;
    Zeus::Renderer m_renderer;

    VkFence oneTimeSubmitFence{ VK_NULL_HANDLE };
    VkCommandPool oneTimeSubmitCommandPool{ VK_NULL_HANDLE };
    VkCommandBuffer oneTimeSubmitCommandBuffer{ VK_NULL_HANDLE };

    EngineStatistics statistics;

    UIManager uiManager;

    DescriptorAllocator globalDescriptorAllocator;

    VkDescriptorSet _drawImageDescriptors;
    VkDescriptorSetLayout _drawImageDescriptorLayout;

    VkPipeline _gradientPipeline;
    VkPipelineLayout _gradientPipelineLayout;

    // std::vector<ComputeEffect> backgroundEffects;

    float renderScale{ 1.f };

    bool stopRendering{ false };
    bool resizeRequested{ false };
};
}
