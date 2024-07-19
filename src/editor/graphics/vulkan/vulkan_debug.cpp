#include "vulkan_debug.hpp"

#include "vulkan_settings.hpp"
#include "vulkan_utils.hpp"

#include <core/logger.hpp>

#include <vulkan/vulkan_core.h>

#include <vector>

namespace Zeus
{
bool createVkDebugUtilsMessengerEXT(
    const VkInstance& instance,
    VkDebugUtilsMessengerEXT& debugMessenger)
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = DEBUG_MESSAGE.MESSAGE_SEVERITY;
    createInfo.messageType = DEBUG_MESSAGE.MESSAGE_TYPE;
    createInfo.pfnUserCallback = debugCallback;

    VkResult result{ createDebugUtilsMessengerEXT(
        instance,
        &createInfo,
        nullptr,
        &debugMessenger) };

    if (result != VK_SUCCESS)
    {
        error("Failed to create debug messenger. {}", vkResultToString(result));
    }

    return result == VK_SUCCESS;
}

bool areValidationLayersSupported(
    const std::vector<const char*>& validationLayers)
{
    std::uint32_t layerCount{ 0 };
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers)
    {
        bool layerFound{ false };

        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            error("{} validation layer not supported", layerName);
            return false;
        }
    }

    return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    [[maybe_unused]] void* pUserData)
{
    switch (messageSeverity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        info("{}", pCallbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        warning("{}", pCallbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        error("{}", pCallbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
        break;
    }

    return VK_FALSE;
}
}
