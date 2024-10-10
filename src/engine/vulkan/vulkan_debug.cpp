#include "vulkan_debug.hpp"

#include "logging/logger.hpp"
#include "math/definitions.hpp"

#include <vulkan/vulkan.h>

#include <cassert>
#include <cstdint>
#include <cstring>

namespace Zeus
{
PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT{ nullptr };

PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT{ nullptr };
PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT{ nullptr };
PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT{ nullptr };

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
        LOG_INFO("{}", pCallbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        LOG_WARNING("{}", pCallbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        LOG_ERROR("{}", pCallbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
        break;
    }

    return VK_FALSE;
}

VkResult createDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance,
        "vkCreateDebugUtilsMessengerEXT");

    if (func != nullptr)
    {
        vkSetDebugUtilsObjectNameEXT =
            reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(
                vkGetInstanceProcAddr(
                    instance,
                    "vkSetDebugUtilsObjectNameEXT"));
        assert(vkSetDebugUtilsObjectNameEXT != nullptr);

        vkCmdBeginDebugUtilsLabelEXT =
            reinterpret_cast<PFN_vkCmdBeginDebugUtilsLabelEXT>(
                vkGetInstanceProcAddr(
                    instance,
                    "vkCmdBeginDebugUtilsLabelEXT"));
        assert(vkCmdBeginDebugUtilsLabelEXT != nullptr);

        vkCmdEndDebugUtilsLabelEXT =
            reinterpret_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(
                vkGetInstanceProcAddr(instance, "vkCmdEndDebugUtilsLabelEXT"));
        assert(vkCmdEndDebugUtilsLabelEXT != nullptr);

        vkCmdInsertDebugUtilsLabelEXT =
            reinterpret_cast<PFN_vkCmdInsertDebugUtilsLabelEXT>(
                vkGetInstanceProcAddr(
                    instance,
                    "vkCmdInsertDebugUtilsLabelEXT"));
        assert(vkCmdInsertDebugUtilsLabelEXT != nullptr);

        return func(instance, pCreateInfo, pAllocator, pMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void destroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT messenger,
    const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance,
        "vkDestroyDebugUtilsMessengerEXT");

    assert(func != nullptr);
    func(instance, messenger, pAllocator);
}

void setDebugUtilsObjectNameEXT(
    VkDevice device,
    VkObjectType objectType,
    std::uint64_t objectHandle,
    const char* pObjectName)
{
    VkDebugUtilsObjectNameInfoEXT nameInfo{};
    nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    nameInfo.objectType = objectType;
    nameInfo.objectHandle = objectHandle;
    nameInfo.pObjectName = pObjectName;

    vkSetDebugUtilsObjectNameEXT(device, &nameInfo);
}

void cmdBeginDebugUtilsLabelEXT(
    VkCommandBuffer commandBuffer,
    const char* pLabelName,
    const Vector4f& color)
{
    VkDebugUtilsLabelEXT labelInfo{};
    labelInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    labelInfo.pLabelName = pLabelName;
    memcpy(&labelInfo.color, &color[0], sizeof(Vector4f));

    vkCmdBeginDebugUtilsLabelEXT(commandBuffer, &labelInfo);
}

void cmdEndDebugUtilsLabelEXT(VkCommandBuffer commandBuffer)
{
    vkCmdEndDebugUtilsLabelEXT(commandBuffer);
}

void cmdInsertDebugUtilsLabelEXT(
    VkCommandBuffer commandBuffer,
    const char* pLabelName,
    const Vector4f& color)
{
    VkDebugUtilsLabelEXT labelInfo{};
    labelInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    labelInfo.pLabelName = pLabelName;
    memcpy(&labelInfo.color, &color[0], sizeof(Vector4f));

    vkCmdInsertDebugUtilsLabelEXT(commandBuffer, &labelInfo);
}
}
