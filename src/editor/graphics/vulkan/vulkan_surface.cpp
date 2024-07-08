#include "vulkan_surface.hpp"

#include "vulkan_utils.hpp"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

namespace Zeus
{
bool createVkSurfaceKHR(
    const VkInstance& instance,
    GLFWwindow* window,
    VkSurfaceKHR& surface)
{
    VkResult result{
        glfwCreateWindowSurface(instance, window, nullptr, &surface)
    };

    if (result != VK_SUCCESS)
    {
        fatal("Failed to create window surface. {}", vkResultToString(result));
    }

    return result == VK_SUCCESS;
}
}
