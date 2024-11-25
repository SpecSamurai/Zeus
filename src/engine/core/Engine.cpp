#include "Engine.hpp"

#include "logging/logger.hpp"
#include "rendering/Renderer.hpp"
#include "vulkan/VkContext.hpp"
#include "window/Window.hpp"

namespace Zeus
{
Renderer* Engine::s_renderer{ nullptr };

void Engine::Initialize(const Window& window)
{
    VkContext::Initialize(window);
    s_renderer = new Renderer(window);
    s_renderer->Initialize();
}

void Engine::Shutdown()
{
    LOG_DEBUG("Shutting down the engine.");

    VkContext::GetDevice().Wait();

    s_renderer->Destroy();
    delete s_renderer;
    VkContext::Destroy();
}

void Engine::Update()
{
    // s_renderer.Update();
}

Renderer& Engine::GetRenderer()
{
    return *s_renderer;
}
}
