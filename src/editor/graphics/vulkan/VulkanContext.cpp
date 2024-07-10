#include "VulkanContext.hpp"

namespace Zeus
{
void VulkanContext::glfwKeyCallback(
    GLFWwindow* window,
    int key,
    [[maybe_unused]] int scancode,
    int action,
    [[maybe_unused]] int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void VulkanContext::glfwCursorPositionCallback(
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

void VulkanContext::glfwScrollCallback(
    [[maybe_unused]] GLFWwindow* window,
    [[maybe_unused]] double xoffset,
    [[maybe_unused]] double yoffset)
{
    // trace("Mouse scroll x:{}, y:{}", xoffset, yoffset);
}

void VulkanContext::framebufferResizeCallback(
    GLFWwindow* window,
    [[maybe_unused]] int width,
    [[maybe_unused]] int height)
{
    auto app = reinterpret_cast<Zeus::VulkanContext*>(
        glfwGetWindowUserPointer(window));
    app->framebufferResized = true;

    // added to make window draw while resizing
    // app->RecreateSwapchain();
    // app->DrawFrame();
}
}
