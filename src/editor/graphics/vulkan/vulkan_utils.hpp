#pragma once

#include <core/logger.hpp>

#include <vulkan/vulkan.h>

namespace Zeus
{
std::uint32_t findMemoryType(
    const VkPhysicalDevice& physicalDevice,
    std::uint32_t typeFilter,
    VkMemoryPropertyFlags properties);

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
