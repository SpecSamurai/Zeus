#include "vulkan_instance.hpp"

#define GLFW_INCLUDE_VULKAN

#include "vulkan_debug.hpp"
#include "vulkan_settings.hpp"
#include "vulkan_utils.hpp"

#include <core/logger.hpp>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

namespace Zeus
{
// Global means that they apply to the entire program and not a specific device
const std::vector<const char*> getRequiredGlobalExtensions();

bool createVkInstance(VkInstance& instance)
{
#ifndef NDEBUG
    if (!areValidationLayerSupported(VALIDATION_LAYERS))
    {
        error("Validation layers requested, but not available");
        return false;
    }
#endif

    VkApplicationInfo applicationInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Zeus",
        .applicationVersion = APPLICATION_VERSION,
        .pEngineName = "Zeus Engine",
        .engineVersion = APPLICATION_VERSION,
        .apiVersion = VK_API_VERSION_1_3,
    };

    auto extensions = getRequiredGlobalExtensions();

    VkInstanceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<std::uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };

#ifndef NDEBUG
    VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = MESSAGE_SEVERITY,
        .messageType = MESSAGE_TYPE,
        .pfnUserCallback = debugCallback,
    };

    createInfo.enabledLayerCount =
        static_cast<std::uint32_t>(VALIDATION_LAYERS.size());
    createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
    createInfo.pNext = &debugMessengerCreateInfo;
#endif

    VkResult result{vkCreateInstance(&createInfo, nullptr, &instance)};
    if (result != VK_SUCCESS)
    {
        error("Instance failed to create. {}", vkResultToString(result));
        return false;
    }

    return true;
}

const std::vector<const char*> getRequiredGlobalExtensions()
{
    std::uint32_t glfwExtensionsCount{0};
    const char** glfwExtensions{
        glfwGetRequiredInstanceExtensions(&glfwExtensionsCount)};

    std::vector<const char*> extensions(
        glfwExtensions,
        glfwExtensions + glfwExtensionsCount);

#ifndef NDEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    return extensions;
}
}
