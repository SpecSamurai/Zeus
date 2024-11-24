#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cstdint>

namespace Zeus
{
struct WindowProperties
{
    const char* title;
    std::uint32_t width;
    std::uint32_t height;
};

class Window
{
public:
    Window(const WindowProperties& properties);

    void Initialize();
    void Destroy();

    void Update();

    void* GetHandle() const;
    const char* GetTitle() const;
    std::uint32_t GetWidth() const;
    std::uint32_t GetHeight() const;

private:
    GLFWwindow* m_handle{ nullptr };

    struct WindowData
    {
        const char* title;
        std::uint32_t width;
        std::uint32_t height;
    } m_data;
};
}
