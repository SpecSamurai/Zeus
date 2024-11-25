#pragma once

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

private:
    static Renderer* s_renderer;
};
}
