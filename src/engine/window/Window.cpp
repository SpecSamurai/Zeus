#include "Window.hpp"

#include "core/logger.hpp"
#include "window/glfw_utils.hpp"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include <cstdint>

namespace Zeus
{
Window::Window(std::uint32_t width, std::uint32_t height, const char* title)
    : title{ title },
      extent{ .width = width, .height = height }
{
}

void Window::Init()
{
    handle = createGlfwWindow(
        static_cast<int>(extent.width),
        static_cast<int>(extent.height),
        title);

    glfwSetWindowUserPointer(handle, this);
    glfwSetFramebufferSizeCallback(handle, framebufferResizeCallback);
    glfwSetWindowIconifyCallback(handle, windowIconifyCallback);
}

void Window::Destroy()
{
    debug("Destroying window: {}", title);

    glfwDestroyWindow(handle);
    glfwTerminate();
}

void Window::framebufferResizeCallback(
    GLFWwindow* window,
    int width,
    int height)
{
    auto obj = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

    debug("framebufferResizeCallback: {}, {}", width, height);

    obj->extent.width = static_cast<std::uint32_t>(width);
    obj->extent.width = static_cast<std::uint32_t>(height);
    obj->resized = true;
}

void Window::windowIconifyCallback(GLFWwindow* window, int iconified)
{
    auto obj = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

    if (iconified)
    {
        debug("iconified: {}, {}, {}", true, 0, 0);

        obj->extent.width = 0;
        obj->extent.width = 0;
        obj->resized = true;
    }
    else
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);

        debug("restored: {}, {}", width, height);

        obj->extent.width = static_cast<std::uint32_t>(width);
        obj->extent.width = static_cast<std::uint32_t>(height);
        obj->resized = true;
    }
}
}
