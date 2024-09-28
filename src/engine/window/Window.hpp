#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include <cstdint>

namespace Zeus
{
class Window
{
public:
    Window(std::uint32_t width, std::uint32_t height, const char* title);
    void Init();
    void Destroy();

    GLFWwindow* handle{ nullptr };
    const char* title;
    VkExtent2D extent;
    bool resized{ false };

private:
    static void framebufferResizeCallback(
        GLFWwindow* window,
        int width,
        int height);

    static void windowIconifyCallback(GLFWwindow* window, int iconified);
};
}
