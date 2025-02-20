#include "Engine.hpp"

#include "ecs/Registry.hpp"
#include "logging/logger.hpp"
#include "rendering/Renderer.hpp"
#include "rhi/VkContext.hpp"
#include "window/Window.hpp"

namespace Zeus
{
Renderer* Engine::s_renderer{ nullptr };
ECS::Registry* Engine::s_world{ nullptr };

void Engine::Initialize(const Window& window)
{
    VkContext::Initialize(window);

    s_world = new ECS::Registry();
    s_renderer = new Renderer(window);
    s_renderer->Initialize();
}

void Engine::Shutdown()
{
    LOG_DEBUG("Shutting down the engine.");

    s_renderer->Destroy();
    delete s_renderer;
    delete s_world;
    VkContext::Destroy();
}

void Engine::Update()
{
    s_renderer.Update();
}

Renderer& Engine::GetRenderer()
{
    return *s_renderer;
}

ECS::Registry& Engine::GetWorld()
{
    return *s_world;
}
}
