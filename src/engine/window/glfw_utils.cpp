#include "glfw_utils.hpp"

#include "logging/logger.hpp"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <vector>

namespace Zeus
{
static bool s_isGLFWInitialzied{ false };

void glfwErrorCallback(int error, const char* description)
{
    LOG_ERROR("Error {}: {}", error, description);
}

GLFWwindow* createGlfwWindow(int width, int height, const char* title)
{
    if (!s_isGLFWInitialzied)
    {
#ifndef NDEBUG
        glfwSetErrorCallback(glfwErrorCallback);
#endif

        if (!glfwInit())
        {
            LOG_ERROR("GLFW initialization failed");
            return nullptr;
        }
        s_isGLFWInitialzied = true;

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }

    GLFWwindow* window{
        glfwCreateWindow(width, height, title, nullptr, nullptr)
    };

    if (!window)
    {
        LOG_ERROR("GLFW Window creation failed");
        glfwTerminate();
        return nullptr;
    }

    return window;
}

const std::vector<const char*> getRequiredGlobalExtensions()
{
    std::uint32_t glfwExtensionsCount{ 0 };
    const char** glfwExtensions{
        glfwGetRequiredInstanceExtensions(&glfwExtensionsCount),
    };

    std::vector<const char*> extensions(
        glfwExtensions,
        glfwExtensions + glfwExtensionsCount);

#ifndef NDEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    return extensions;
}
}
