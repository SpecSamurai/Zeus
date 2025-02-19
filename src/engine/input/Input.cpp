#include "Input.hpp"

#include "KeyCode.hpp"
#include "MouseButtonCode.hpp"
#include "application/Application.hpp"
#include "math/definitions.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cstdint>

namespace Zeus
{
bool Input::IsKeyDown(const KeyCode keyCode)
{
    auto* window{ static_cast<GLFWwindow*>(
        Application::Instance().GetWindow().GetHandle()) };
    auto state{ glfwGetKey(window, static_cast<std::int32_t>(keyCode)) };

    return state == GLFW_PRESS;
}

bool Input::IsKeyUp(const KeyCode keyCode)
{
    auto* window{ static_cast<GLFWwindow*>(
        Application::Instance().GetWindow().GetHandle()) };
    auto state{ glfwGetKey(window, static_cast<std::int32_t>(keyCode)) };

    return state == GLFW_RELEASE;
}

bool Input::IsMouseButtonDown(const MouseButtonCode buttonCode)
{
    auto* window{ static_cast<GLFWwindow*>(
        Application::Instance().GetWindow().GetHandle()) };
    auto state{
        glfwGetMouseButton(window, static_cast<std::int32_t>(buttonCode))
    };

    return state == GLFW_PRESS;
}

bool Input::IsMouseButtonUp(const MouseButtonCode buttonCode)
{
    auto* window{ static_cast<GLFWwindow*>(
        Application::Instance().GetWindow().GetHandle()) };
    auto state{
        glfwGetMouseButton(window, static_cast<std::int32_t>(buttonCode))
    };

    return state == GLFW_RELEASE;
}

Math::Vector2f Input::GetMousePosition()
{
    auto* window{ static_cast<GLFWwindow*>(
        Application::Instance().GetWindow().GetHandle()) };

    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);

    return Math::Vector2f(static_cast<float>(xPos), static_cast<float>(yPos));
}
}
