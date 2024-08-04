#pragma once

#include <vulkan/vulkan_core.h>

namespace Zeus
{
VKAPI_ATTR VkBool32 VKAPI_CALL defaultDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    [[maybe_unused]] void* pUserData);

struct Instance
{
    VkInstance handle{ VK_NULL_HANDLE };
#ifndef NDEBUG
    VkDebugUtilsMessengerEXT debugUtilsMessenger{ VK_NULL_HANDLE };
#endif
};

void destroyInstance(Instance& instance);
}
