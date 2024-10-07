#pragma once

#include "ui/UIManager.hpp"

#include <renderer/Renderer.hpp>
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

private:
    void Draw();

    void InitCompute();

private:
    Zeus::Window m_window;
    Zeus::VulkanContext m_vkContext;
    Zeus::Renderer m_renderer;

    UIManager uiManager;

    DescriptorAllocator computeDescriptorAllocator;

    VkDescriptorSet _drawImageDescriptors;
    VkDescriptorSetLayout _drawImageDescriptorLayout;

    VkPipeline _gradientPipeline;
    VkPipelineLayout _gradientPipelineLayout;

    // std::vector<ComputeEffect> backgroundEffects;

    float renderScale{ 1.f };

    EngineStatistics statistics;

    // bool stopRendering{ false };
};
}
