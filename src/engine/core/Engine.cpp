#include "Engine.hpp"

#include "components/Renderable.hpp"
#include "core/World.hpp"
#include "logging/logger.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/Renderer_types.hpp"
#include "rhi/VkContext.hpp"
#include "window/Window.hpp"

namespace Zeus
{
Renderer* Engine::s_renderer{ nullptr };
World* Engine::s_world{ nullptr };

void Engine::Initialize(const Window& window)
{
    VkContext::Initialize(window);

    s_world = new class World();
    s_renderer = new class Renderer(window);
    s_renderer->Initialize();
}

void Engine::Shutdown()
{
    LOG_DEBUG("Shutting down the engine.");

    s_renderer->Destroy();

    VkContext::Destroy();

    delete s_renderer;
    delete s_world;

    s_renderer = nullptr;
    s_world = nullptr;
}

void Engine::Update()
{
    s_world->Update();

    auto query{ s_world->Registry().QueryAll<Renderable>() };
    s_renderer->SetEntities(RendererEntity::MESH_OPAQUE, query);

    s_renderer->Update();
}

Renderer& Engine::Renderer()
{
    return *s_renderer;
}

World& Engine::World()
{
    return *s_world;
}
}
