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
    void Init();
    void Destroy();

    void Update();

    inline constexpr std::uint32_t GetWidth() const
    {
        return m_data.width;
    }

    inline constexpr std::uint32_t GetHeight() const
    {
        return m_data.height;
    }

    inline const char* GetTitle() const
    {
        return m_data.title;
    }

    inline void* GetHandle() const
    {
        return m_handle;
    }

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
