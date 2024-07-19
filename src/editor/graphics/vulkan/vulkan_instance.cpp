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
const std::vector<const char*> getRequiredGlobalExtensions();

bool createVkInstance(VkInstance& instance)
{
#ifndef NDEBUG
    if (!areValidationLayersSupported(VALIDATION_LAYERS))
    {
        error("Validation layers requested, but not available");
        return false;
    }
#endif

    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = "Zeus";
    applicationInfo.applicationVersion = APPLICATION_VERSION;
    applicationInfo.pEngineName = "Zeus Engine";
    applicationInfo.engineVersion = APPLICATION_VERSION;
    applicationInfo.apiVersion = VK_API_VERSION_1_3;

    auto extensions = getRequiredGlobalExtensions();

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &applicationInfo;
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = nullptr;
    createInfo.enabledExtensionCount =
        static_cast<std::uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

#ifndef NDEBUG
    VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};
    debugMessengerCreateInfo.sType =
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugMessengerCreateInfo.messageSeverity = DEBUG_MESSAGE.MESSAGE_SEVERITY;
    debugMessengerCreateInfo.messageType = DEBUG_MESSAGE.MESSAGE_TYPE;
    debugMessengerCreateInfo.pfnUserCallback = debugCallback;

    createInfo.enabledLayerCount =
        static_cast<std::uint32_t>(VALIDATION_LAYERS.size());
    createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
    createInfo.pNext = &debugMessengerCreateInfo;
#endif

    VkResult result{ vkCreateInstance(&createInfo, nullptr, &instance) };
    if (result != VK_SUCCESS)
    {
        error("Instance failed to create. {}", vkResultToString(result));
    }

    return result == VK_SUCCESS;
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
