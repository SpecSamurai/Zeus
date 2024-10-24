#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
struct Instance
{
    VkInstance handle;
    VkDebugUtilsMessengerEXT debugUtilsMessenger;
};

class InstanceBuilder
{
public:
    Instance Build();

    void SetAppName(const char* name);
    void SetEngineName(const char* name);
    void SetApplicationVersion(std::uint32_t applicationVersion);
    void SetEngineVersion(std::uint32_t engineVersion);
    void SetApiVersion(std::uint32_t apiVersion);
    void SetExtensions(const std::vector<const char*>& extensions);

    void SetValidationLayers(const std::vector<const char*>& validationLayers);
    void SetDebugCallback(PFN_vkDebugUtilsMessengerCallbackEXT callback);
    void SetDebugMessageSeverity(
        VkDebugUtilsMessageSeverityFlagsEXT messageSeverity);
    void SetDebugMessageType(VkDebugUtilsMessageTypeFlagsEXT messageType);
    void SetUserData(void* userData);

private:
    bool Validate();

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
