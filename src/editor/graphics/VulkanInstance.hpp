#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <vector>

namespace Zeus
{
void CreateInstance(VkInstance& instance, const char* _title);

void SetupDebugMessenger(
    VkInstance& instance,
    VkDebugUtilsMessengerEXT& debugMessenger);

bool checkValidationLayerSupport(
    const std::vector<const char*>& validationLayers);

std::vector<const char*> getRequiredExtensions();

void populateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT& createInfo);

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    [[maybe_unused]] void* pUserData);
}
