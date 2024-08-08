#pragma once

#include "math/definitions.hpp"

#include <vulkan/vulkan.h>

namespace Zeus
{
VKAPI_ATTR VkBool32 VKAPI_CALL defaultDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    [[maybe_unused]] void* pUserData);

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
    VkDevice device,
    VkObjectType objectType,
    uint64_t objectHandle,
    const char* pObjectName);

void cmdBeginDebugUtilsLabelEXT(
    VkCommandBuffer commandBuffer,
    const char* pLabelName,
    const Vector4f& color);

void cmdEndDebugUtilsLabelEXT(VkCommandBuffer commandBuffer);

void cmdInsertDebugUtilsLabelEXT(
    VkCommandBuffer commandBuffer,
    const char* pLabelName,
    const Vector4f& color);
}
