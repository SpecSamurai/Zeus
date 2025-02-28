#pragma once

#include "CommandLineArgs.hpp"
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
    virtual ~Application();

    virtual void Initialize() = 0;
    virtual void Run() = 0;
    virtual void Shutdown() = 0;

    static Application& Instance();
    Window& Window();
    bool IsRunning() const;
    bool IsMinimized() const;

private:
    bool OnWindowClosed(const WindowClosedEvent& event);
    bool OnWindowResized(const WindowResizedEvent& event);

private:
    static Application* s_instance;

    class Window m_window;

    bool m_running{ false };
    bool m_minimized{ false };
};

[[nodiscard]] extern Application* CreateApplication(CommandLineArgs args);
}
