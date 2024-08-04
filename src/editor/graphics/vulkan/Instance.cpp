#include "Instance.hpp"

#include "vulkan_utils.hpp"

#include <core/logger.hpp>

namespace Zeus
{
VKAPI_ATTR VkBool32 VKAPI_CALL defaultDebugCallback(
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

void destroyInstance(Instance& instance)
{
#ifndef NDEBUG
    destroyDebugUtilsMessengerEXT(
        instance.handle,
        instance.debugUtilsMessenger,
        nullptr);
#endif

    vkDestroyInstance(instance.handle, nullptr);
}
}
