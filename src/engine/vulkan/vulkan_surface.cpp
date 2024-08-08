#include "vulkan_surface.hpp"

#include "vulkan_utils.hpp"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

namespace Zeus
{
VkResult createVkSurfaceKHR(
    const VkInstance& instance,
    GLFWwindow* window,
    VkSurfaceKHR& surface)
{
    VkResult result{
        glfwCreateWindowSurface(instance, window, nullptr, &surface)
    };

    CHECK_VKRESULT(result, "Failed to create window surface.");

    return result;
}
}
