#pragma once

#include "ecs/Registry.hpp"
#include "rendering/Renderer.hpp"
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
    static ECS::Registry& GetWorld();

private:
    static Renderer* s_renderer;
    static ECS::Registry* s_world;
};
}
