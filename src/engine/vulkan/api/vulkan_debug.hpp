#pragma once

#include "logging/logger.hpp"
#include "math/definitions.hpp"

#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

#include <cassert>
#include <cstdint>
#include <vector>
#ifdef NDEBUG
#include <tuple>
#endif

namespace Zeus
{
namespace DebugLabelColors
{
const static Color RED{ Vector4f(1.f, 0.f, 0.f, 1.f) };
const static Color GREEN{ Vector4f(0.f, 1.f, 0.f, 1.f) };
}

#ifdef NDEBUG
#define VKCHECK(expression, msg) std::ignore = expression
#else
#define VKCHECK(expression, msg)                                               \
    do                                                                         \
    {                                                                          \
        VkResult checkVkResult{ expression };                                  \
        if (checkVkResult != VK_SUCCESS)                                       \
        {                                                                      \
            LOG_FATAL("{} {}", msg, string_VkResult(checkVkResult));           \
            assert(checkVkResult == VK_SUCCESS);                               \
        }                                                                      \
    } while (0)
#endif

inline static const std::vector<const char*> VALIDATION_LAYERS{
    "VK_LAYER_KHRONOS_validation",
};

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
