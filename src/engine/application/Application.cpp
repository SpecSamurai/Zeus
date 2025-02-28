#include "Application.hpp"

#include "events/Event.hpp"
#include "window/Window.hpp"

#include <cassert>

namespace Zeus
{
Application* Application::s_instance{ nullptr };

Application::Application(const ApplicationSpecification& specification)
    : m_window(WindowProperties{
          .title = specification.name,
          .width = specification.windowWidth,
          .height = specification.windowHeight,
      })
{
    assert(!s_instance && "Application already created.");
    s_instance = this;

    Event::Dispatcher.Register<WindowClosedEvent>(
        "Application::WindowClosedEvent",
        [this](const WindowClosedEvent& event) -> bool {
            return OnWindowClosed(event);
        });

    Event::Dispatcher.Register<WindowResizedEvent>(
        "Application::WindowResizedEvent",
        [this](const WindowResizedEvent& event) -> bool {
            return OnWindowResized(event);
        });

    m_window.Initialize();
    m_running = true;
}

Application::~Application()
{
    m_window.Destroy();
}

Application& Application::Instance()
{
    assert(s_instance && "Application instance is not initialized.");
    return *s_instance;
}

Window& Application::Window()
{
    return m_window;
}

bool Application::IsRunning() const
{
    return m_running;
}

bool Application::IsMinimized() const
{
    return m_minimized;
}

bool Application::OnWindowClosed(
    [[maybe_unused]] const WindowClosedEvent& event)
{
    m_running = false;
    return true;
}

bool Application::OnWindowResized(const WindowResizedEvent& event)
{
    m_minimized = event.width == 0 || event.height == 0;
    return true;
}
}
