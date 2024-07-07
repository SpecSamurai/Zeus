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

    return window;
}
}
