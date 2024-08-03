#pragma once

#include "Instance.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <optional>
#include <vector>

namespace Zeus
{
class InstanceBuilder
{
public:
    std::optional<Instance> build();

    InstanceBuilder& setAppName(const char* name);
    InstanceBuilder& setEngineName(const char* name);
    InstanceBuilder& setApplicationVersion(std::uint32_t applicationVersion);
    InstanceBuilder& setEngineVersion(std::uint32_t engineVersion);
    InstanceBuilder& setApiVersion(std::uint32_t apiVersion);
    InstanceBuilder& setExtensions(const std::vector<const char*>& extensions);

#ifndef NDEBUG
    InstanceBuilder& setValidationLayers(
        const std::vector<const char*>& validationLayers);

    InstanceBuilder& setDebugCallback(
        PFN_vkDebugUtilsMessengerCallbackEXT callback);

    InstanceBuilder& setDebugMessageSeverity(
        VkDebugUtilsMessageSeverityFlagsEXT messageSeverity);

    InstanceBuilder& setDebugMessageType(
        VkDebugUtilsMessageTypeFlagsEXT messageType);

    InstanceBuilder& setUserData(void* userData);
#endif

private:
    bool validate();

    struct InstanceInfo
    {
        const char* applicationName{ nullptr };
        const char* engineName{ nullptr };
        std::uint32_t applicationVersion{ 0 };
        std::uint32_t engineVersion{ 0 };
        std::uint32_t apiVersion{ 0 };

        std::vector<const char*> extensions{};

#ifndef NDEBUG
        std::vector<const char*> validationLayers{};
        PFN_vkDebugUtilsMessengerCallbackEXT debugCallback{};
        VkDebugUtilsMessageSeverityFlagsEXT debugMessageSeverity{};
        VkDebugUtilsMessageTypeFlagsEXT debugMessageType{};
        void* userData{ nullptr };
#endif
    } info;
};

inline constexpr struct InstanceDefaultConfig
{
    inline static constexpr std::uint32_t API_VERSION{ VK_API_VERSION_1_3 };

    inline static const std::vector<const char*> VALIDATION_LAYERS = {
        "VK_LAYER_KHRONOS_validation",
    };

    inline static constexpr struct DebugMessage
    {
        inline static constexpr VkDebugUtilsMessageSeverityFlagsEXT
            MESSAGE_SEVERITY{ VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                              // VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT };

        inline static constexpr VkDebugUtilsMessageTypeFlagsEXT MESSAGE_TYPE{
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
        };
    } DEBUG_MESSAGE;
} INSTANCE_DEFAULT;
}
