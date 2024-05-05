#pragma once

#include <vulkan/vulkan_core.h>

#include <vector>

namespace Zeus
{
bool createVkDebugUtilsMessengerEXT(
    const VkInstance& instance,
    VkDebugUtilsMessengerEXT& debugMessenger);

bool areValidationLayerSupported(
    const std::vector<const char*>& validationLayers);

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    [[maybe_unused]] void* pUserData);
}
