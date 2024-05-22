#include "glfw_utils.hpp"

#include <core/logger.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Zeus
{
void glfwErrorCallback(int error, const char* description)
{
    Zeus::error("Error {}: {}", error, description);
}

inline void glfwKeyCallback(
    GLFWwindow* window,
    int key,
    [[maybe_unused]] int scancode,
    int action,
    [[maybe_unused]] int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

inline void glfwCursorPositionCallback(
    [[maybe_unused]] GLFWwindow* window,
    [[maybe_unused]] double xpos,
    [[maybe_unused]] double ypos)
{
    //   bool firstMouse = true;
    //   float x = static_cast<float>(xpos);
    //   float y = static_cast<float>(ypos);

    //   if (firstMouse)
    //   {
    //       lastX = xpos;
    //       lastY = ypos;
    //       firstMouse = false;
    //   }

    //   float xoffset = xpos - lastX;
    //   float yoffset =
    //       lastY - ypos; // reversed since y-coordinates go from bottom to
    //       top

    //   lastX = xpos;
    //   lastY = ypos;

    // trace("Mouse position x:{}, y:{}", xpos, ypos);
}

inline void glfwScrollCallback(
    [[maybe_unused]] GLFWwindow* window,
    double xoffset,
    double yoffset)
{
    trace("Mouse scroll x:{}, y:{}", xoffset, yoffset);
}

GLFWwindow* createGlfwWindow(int width, int height, const char* title)
{
#ifndef NDEBUG
    glfwSetErrorCallback(glfwErrorCallback);
#endif

    if (!glfwInit())
    {
        error("GLFW initialization failed");
        return nullptr;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window{
        glfwCreateWindow(width, height, title, nullptr, nullptr)
    };

    if (!window)
    {
        error("GLFW Window creation failed");
        glfwTerminate();
        return nullptr;
    }

    // glfwSetKeyCallback(window, glfwKeyCallback);
    // glfwSetCursorPosCallback(window, glfwCursorPositionCallback);
    // glfwSetScrollCallback(window, glfwScrollCallback);

    // glfwSetWindowUserPointer(window, this);
    // glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    //
    // lastTime = glfwGetTime();
    return window;
}
}
