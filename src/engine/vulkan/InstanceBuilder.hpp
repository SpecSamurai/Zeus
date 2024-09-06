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

    void setAppName(const char* name);
    void setEngineName(const char* name);
    void setApplicationVersion(std::uint32_t applicationVersion);
    void setEngineVersion(std::uint32_t engineVersion);
    void setApiVersion(std::uint32_t apiVersion);
    void setExtensions(const std::vector<const char*>& extensions);

    void setValidationLayers(const std::vector<const char*>& validationLayers);
    void setDebugCallback(PFN_vkDebugUtilsMessengerCallbackEXT callback);
    void setDebugMessageSeverity(
        VkDebugUtilsMessageSeverityFlagsEXT messageSeverity);
    void setDebugMessageType(VkDebugUtilsMessageTypeFlagsEXT messageType);
    void setUserData(void* userData);

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

        std::vector<const char*> validationLayers{};
        PFN_vkDebugUtilsMessengerCallbackEXT debugCallback{};
        VkDebugUtilsMessageSeverityFlagsEXT debugMessageSeverity{};
        VkDebugUtilsMessageTypeFlagsEXT debugMessageType{};
        void* userData{ nullptr };
    } info;
};
}
