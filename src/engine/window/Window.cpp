#include "Window.hpp"

#include "events/KeyEvent.hpp"
#include "events/MouseEvent.hpp"
#include "events/WindowEvent.hpp"
#include "input/MouseButtonCode.hpp"
#include "logging/logger.hpp"
#include "math/definitions.hpp"
#include "window/glfw_utils.hpp"

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

            // EventDispatcher.Dispatch(WindowResizedEvent{
            //     .width = windowData->width,
            //     .height = windowData->height,
            // });
        });

    glfwSetWindowCloseCallback(
        m_handle,
        []([[maybe_unused]] GLFWwindow* window) {
            // EventDispatcher.Dispatch(WindowClosedEvent{});
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
            case GLFW_PRESS: {
                // EventDispatcher.Dispatch(KeyPressedEvent{
                //     .keyCode = static_cast<KeyCode>(key),
                //     .isRepeated = false,
                // });
                break;
            }
            case GLFW_RELEASE: {
                // EventDispatcher.Dispatch(KeyReleasedEvent{
                //     .keyCode = static_cast<KeyCode>(key),
                // });
                break;
            }
            case GLFW_REPEAT: {
                // EventDispatcher.Dispatch(KeyPressedEvent{
                //     .keyCode = static_cast<KeyCode>(key),
                //     .isRepeated = true,
                // });
                break;
            }
            }
        });

    glfwSetCharCallback(
        m_handle,
        []([[maybe_unused]] GLFWwindow* window, unsigned int codepoint) {
            // EventDispatcher.Dispatch(KeyTypedEvent{
            //     .keyCode = static_cast<KeyCode>(codepoint),
            // });
        });

    glfwSetMouseButtonCallback(
        m_handle,
        []([[maybe_unused]] GLFWwindow* window,
           int button,
           int action,
           [[maybe_unused]] int mods) {
            switch (action)
            {
            case GLFW_PRESS: {
                // EventDispatcher.Dispatch(MouseButtonPressedEvent{
                //     .buttonCode = static_cast<MouseButtonCode>(button),
                // });
                break;
            }
            case GLFW_RELEASE: {
                // EventDispatcher.Dispatch(MouseButtonReleasedEvent{
                //     .buttonCode = static_cast<MouseButtonCode>(button),
                // });
                break;
            }
            }
        });

    glfwSetScrollCallback(
        m_handle,
        []([[maybe_unused]] GLFWwindow* window,
           double xOffset,
           double yOffset) {
            // EventDispatcher.Dispatch(MouseScrolledEvent{
            //     .offset = Vector2f(
            //         static_cast<float>(xOffset),
            //         static_cast<float>(yOffset)),
            // });
        });

    glfwSetCursorPosCallback(
        m_handle,
        []([[maybe_unused]] GLFWwindow* window, double xPos, double yPos) {
            // EventDispatcher.Dispatch(MouseMovedEvent{
            //     .position = Vector2f(
            //         static_cast<float>(xPos),
            //         static_cast<float>(yPos)),
            // });
        });
}

void Window::Destroy()
{
    LOG_DEBUG("Destroying window: {}", m_data.title);

    glfwDestroyWindow(m_handle);
    glfwTerminate();
}

void Window::Update()
{
    glfwPollEvents();
}
}
