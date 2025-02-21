#pragma once

#include "ecs/Registry.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/Renderer2.hpp"
#include "window/Window.hpp"

namespace Zeus
{
class Engine
{
public:
    static void Initialize(const Window& window);
    static void Shutdown();
    static void Update();

    static Renderer& GetRenderer();
    static Renderer2& Renderer();
    static ECS::Registry& World();

private:
    static class Renderer2* s_renderer;
    static class Renderer* s_rendererOld;
    static ECS::Registry* s_world;
};
}
