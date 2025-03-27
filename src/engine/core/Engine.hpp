#pragma once

#include "core/World.hpp"
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

    static Renderer& Renderer();
    static World& World();

private:
    static class Renderer* s_renderer;
    static class World* s_world;
};
}
