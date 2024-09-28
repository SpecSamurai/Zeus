#pragma once

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

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
    bool invalidExtent{ false };

private:
    static void framebufferResizeCallback(
        GLFWwindow* window,
        int width,
        int height);

    static void windowIconifyCallback(GLFWwindow* window, int iconified);
};
}
