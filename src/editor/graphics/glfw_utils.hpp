#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Zeus
{
[[nodiscard]] GLFWwindow* createGlfwWindow(
    int width,
    int height,
    const char* title);
}
