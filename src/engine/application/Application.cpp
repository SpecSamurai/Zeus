#include "Application.hpp"

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

    m_window.Initialize();
}

const Application& Application::Instance()
{
    assert(s_instance && "Application instance is not initialized.");
    return *s_instance;
}

const Window& Application::GetWindow() const
{
    return m_window;
}
}
