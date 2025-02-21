#include "Engine.hpp"

#include "ecs/Registry.hpp"
#include "logging/logger.hpp"
#include "rendering/Renderer.hpp"
#include "rhi/VkContext.hpp"
#include "window/Window.hpp"

namespace Zeus
{
Renderer* Engine::s_rendererOld{ nullptr };
Renderer2* Engine::s_renderer{ nullptr };
ECS::Registry* Engine::s_world{ nullptr };

void Engine::Initialize(const Window& window)
{
    VkContext::Initialize(window);

    s_world = new ECS::Registry();

    /*s_rendererOld = new Renderer(window);*/
    /*s_rendererOld->Initialize();*/
    s_renderer = new Renderer2(window);
    s_renderer->Initialize();
}

void Engine::Shutdown()
{
    LOG_DEBUG("Shutting down the engine.");

    /*s_rendererOld->Destroy();*/
    s_renderer->Destroy();
    /*delete s_rendererOld;*/
    delete s_renderer;

    delete s_world;

    VkContext::Destroy();
}

void Engine::Update()
{
    /*s_rendererOld->Draw();*/
    s_renderer->Update();
}

Renderer& Engine::GetRenderer()
{
    return *s_rendererOld;
}

Renderer2& Engine::Renderer()
{
    return *s_renderer;
}

ECS::Registry& Engine::World()
{
    return *s_world;
}
}
