#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <vector>

namespace Zeus
{
[[nodiscard]] GLFWwindow* createGlfwWindow(
    int width,
    int height,
    const char* title);

const std::vector<const char*> getRequiredGlobalExtensions();
}
