#include "Window.hpp"

#include "events/Event.hpp"
#include "events/KeyEvent.hpp"
#include "events/MouseEvent.hpp"
#include "events/WindowEvent.hpp"
#include "glfw_utils.hpp"
#include "input/KeyCode.hpp"
#include "input/MouseButtonCode.hpp"
#include "logging/logger.hpp"
#include "math/definitions.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cstdint>

namespace Zeus
{
Window::Window(const WindowProperties& properties)
    : m_data{ .title = properties.title,
              .width = properties.width,
              .height = properties.height }
{
}

void Window::Init()
{
    m_handle = createGlfwWindow(
        static_cast<int>(m_data.width),
        static_cast<int>(m_data.height),
        m_data.title);

    glfwSetWindowUserPointer(m_handle, &m_data);

    glfwSetWindowSizeCallback(
        m_handle,
        [](GLFWwindow* window, int width, int height) {
            auto windowData{ reinterpret_cast<WindowData*>(
                glfwGetWindowUserPointer(window)) };

            windowData->width = static_cast<std::uint32_t>(width);
            windowData->height = static_cast<std::uint32_t>(height);

            Event::Dispatcher.Dispatch(WindowResizedEvent{
                .width = windowData->width,
                .height = windowData->height,
            });
        });

    glfwSetWindowCloseCallback(
        m_handle,
        []([[maybe_unused]] GLFWwindow* window) {
            Event::Dispatcher.Dispatch(WindowClosedEvent{});
        });

    glfwSetKeyCallback(
        m_handle,
        []([[maybe_unused]] GLFWwindow* window,
           int key,
           [[maybe_unused]] int scancode,
           int action,
           [[maybe_unused]] int mods) {
            switch (action)
            {
            case GLFW_PRESS:
                Event::Dispatcher.Dispatch(KeyPressedEvent{
                    .keyCode = static_cast<KeyCode>(key),
                    .isRepeated = false,
                });
                break;
            case GLFW_RELEASE:
                Event::Dispatcher.Dispatch(KeyReleasedEvent{
                    .keyCode = static_cast<KeyCode>(key),
                });
                break;
            case GLFW_REPEAT:
                Event::Dispatcher.Dispatch(KeyPressedEvent{
                    .keyCode = static_cast<KeyCode>(key),
                    .isRepeated = true,
                });
                break;
            }
        });

    glfwSetCharCallback(
        m_handle,
        []([[maybe_unused]] GLFWwindow* window, unsigned int codepoint) {
            Event::Dispatcher.Dispatch(KeyTypedEvent{
                .keyCode = static_cast<KeyCode>(codepoint),
            });
        });

    glfwSetMouseButtonCallback(
        m_handle,
        []([[maybe_unused]] GLFWwindow* window,
           int button,
           int action,
           [[maybe_unused]] int mods) {
            switch (action)
            {
            case GLFW_PRESS:
                Event::Dispatcher.Dispatch(MouseButtonPressedEvent{
                    .buttonCode = static_cast<MouseButtonCode>(button),
                });
                break;
            case GLFW_RELEASE:
                Event::Dispatcher.Dispatch(MouseButtonReleasedEvent{
                    .buttonCode = static_cast<MouseButtonCode>(button),
                });
                break;
            }
        });

    glfwSetScrollCallback(
        m_handle,
        []([[maybe_unused]] GLFWwindow* window,
           double xOffset,
           double yOffset) {
            Event::Dispatcher.Dispatch(MouseScrolledEvent{
                .offset = Vector2f(
                    static_cast<float>(xOffset),
                    static_cast<float>(yOffset)),
            });
        });

    glfwSetCursorPosCallback(
        m_handle,
        []([[maybe_unused]] GLFWwindow* window, double xPos, double yPos) {
            Event::Dispatcher.Dispatch(MouseMovedEvent{
                .position = Vector2f(
                    static_cast<float>(xPos),
                    static_cast<float>(yPos)),
            });
        });
}

void Window::Destroy()
{
    LOG_DEBUG("Destroying window: {}", m_data.title);

    glfwDestroyWindow(m_handle);
    glfwTerminate();

    m_handle = nullptr;
}

void Window::Update()
{
    if (m_data.width == 0 || m_data.height == 0)
        glfwWaitEvents();
    else
        glfwPollEvents();
}

void* Window::GetHandle() const
{
    return m_handle;
}

const char* Window::GetTitle() const
{
    return m_data.title;
}

std::uint32_t Window::GetWidth() const
{
    return m_data.width;
}

std::uint32_t Window::GetHeight() const
{
    return m_data.height;
}
}
