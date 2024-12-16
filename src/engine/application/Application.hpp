#pragma once

#include "CommandLineArgs.hpp"
#include "camera/EditorCamera.hpp"
#include "camera/camera_utils.hpp"
#include "events/MouseEvent.hpp"
#include "events/WindowEvent.hpp"
#include "window/Window.hpp"

#include <cstdint>

namespace Zeus
{
struct ApplicationSpecification
{
    const char* name;
    CommandLineArgs commandLineArgs;
    std::uint32_t windowWidth;
    std::uint32_t windowHeight;
};

class Application
{
public:
    Application(const ApplicationSpecification& specification);
    void Initialize();
    void Run();
    void Shutdown();

    inline const Window& GetWindow() const
    {
        return m_window;
    }

    inline static const Application& Instance()
    {
        return *s_instance;
    }

private:
    bool OnWindowClosed(const WindowClosedEvent& event);
    bool OnWindowResized(const WindowResizedEvent& event);

    void HandleKeyboard();

    bool OnMouseMoved(const MouseMovedEvent& event);

private:
    static Application* s_instance;

    inline static std::unique_ptr<EditorCamera> camera =
        createFreeflyEditorCamera(1440.f / 1080.f);

    inline static struct MousePosition
    {
        float x;
        float y;
    } m_lastMousePosition;
    inline static bool m_isMouseReleased{ true };

    Window m_window;

    bool m_running{ false };
    bool m_minimized{ false };
};

[[nodiscard]] extern Application* CreateApplication(CommandLineArgs args);
}
