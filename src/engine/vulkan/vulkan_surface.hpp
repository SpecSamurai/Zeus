#pragma once

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

namespace Zeus
{
VkResult createVkSurfaceKHR(
    const VkInstance& instance,
    GLFWwindow* window,
    VkSurfaceKHR& surface);
}
