#pragma once

// #include "ui/UIManager.hpp"

#include "rendering/Renderer.hpp"
#include "vulkan/VulkanContext.hpp"
#include "window/Window.hpp"

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
    Zeus::Window m_window;
    Zeus::VulkanContext m_vkContext;
    Zeus::Renderer m_renderer;

    EngineStatistics statistics;
    // UIManager uiManager;
};
}
