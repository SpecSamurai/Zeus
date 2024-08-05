#include "VulkanContext.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Zeus
{
void VulkanContext::glfwKeyCallback(
    GLFWwindow* window,
    int key,
    [[maybe_unused]] int scancode,
    int action,
    [[maybe_unused]] int mods)
{
    auto app =
        reinterpret_cast<VulkanContext*>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (key == GLFW_KEY_C && action == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        app->ref = (app->ref + 1) % 5;

    if (key == GLFW_KEY_A)
        app->camera.Keyboard('a', 0.1f);
    if (key == GLFW_KEY_D)
        app->camera.Keyboard('d', 0.1f);

    if (key == GLFW_KEY_W)
        app->camera.Keyboard('w', 0.1f);

    if (key == GLFW_KEY_S)
        app->camera.Keyboard('s', 0.1f);
}

void VulkanContext::glfwCursorPositionCallback(
    [[maybe_unused]] GLFWwindow* window,
    [[maybe_unused]] double xpos,
    [[maybe_unused]] double ypos)
{
    auto app =
        reinterpret_cast<VulkanContext*>(glfwGetWindowUserPointer(window));

    if (app->firstMouse)
    {
        app->lastX = xpos;
        app->lastY = ypos;
        app->firstMouse = false;
    }

    float xoffset = xpos - app->lastX;
    float yoffset =
        app->lastY - ypos; // reversed since y-coordinates go from bottom to top

    app->lastX = xpos;
    app->lastY = ypos;

    app->camera.Mouse(xoffset, yoffset);
}

void VulkanContext::glfwScrollCallback(
    [[maybe_unused]] GLFWwindow* window,
    [[maybe_unused]] double xoffset,
    [[maybe_unused]] double yoffset)
{
    // app->camera.Zoom(yOffset);
    // trace("Mouse scroll x:{}, y:{}", xoffset, yoffset);
}

void VulkanContext::framebufferResizeCallback(
    GLFWwindow* window,
    [[maybe_unused]] int width,
    [[maybe_unused]] int height)
{
    auto app =
        reinterpret_cast<VulkanContext*>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;

    // added to make window draw while resizing
    // app->RecreateSwapchain();
    // app->DrawFrame();
}
}
