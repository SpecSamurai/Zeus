#pragma once

#include <core/logger.hpp>

#include <vulkan/vulkan.h>

namespace Zeus
{
VkResult createDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pMessenger);

void destroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT messenger,
    const VkAllocationCallbacks* pAllocator);

void setDebugUtilsObjectNameEXT(
    VkInstance instance,
    VkDevice device,
    const VkDebugUtilsObjectNameInfoEXT* pNameInfo);

const char* vkResultToString(VkResult result);
};
