#include "vulkan_surface.hpp"

#include "vulkan_debug.hpp"
#include "vulkan_memory.hpp"

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
    VkResult result{ glfwCreateWindowSurface(
        instance,
        window,
        allocationCallbacks.get(),
        &surface) };

    VKCHECK(result, "Failed to create window surface.");

    return result;
}
}
