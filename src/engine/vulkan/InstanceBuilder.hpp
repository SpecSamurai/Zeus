#pragma once

#include "Instance.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
class InstanceBuilder
{
public:
    Instance build();

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
}
