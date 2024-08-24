#pragma once

#include "core/logger.hpp"
#include "math/definitions.hpp"

#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

#include <cassert>
#include <cstdint>
#ifdef NDEBUG
#include <tuple>
#endif

namespace Zeus
{
#ifdef NDEBUG
#define VKCHECK(expression, msg) std::ignore = expression
#else
#define VKCHECK(expression, msg)                                               \
    do                                                                         \
    {                                                                          \
        VkResult checkVkResult{ expression };                                  \
        if (checkVkResult != VK_SUCCESS)                                       \
        {                                                                      \
            fatal("{} {}", msg, string_VkResult(checkVkResult));               \
            assert(checkVkResult == VK_SUCCESS);                               \
        }                                                                      \
    } while (0)
#endif

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
    std::uint64_t objectHandle,
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