#include "InstanceBuilder.hpp"

#include "logging/logger.hpp"
#include "rhi/vulkan_debug.hpp"
#include "rhi/vulkan_memory.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
const std::vector<const char*> INSTANCE_EXTENSIONS{
    VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
};

constexpr struct DebugMessage
{
    static constexpr VkDebugUtilsMessageSeverityFlagsEXT MESSAGE_SEVERITY{
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        // VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
    };

    static constexpr VkDebugUtilsMessageTypeFlagsEXT MESSAGE_TYPE{
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
    };
} DEBUG_MESSAGE;

Instance InstanceBuilder::Build()
{
    if (!Validate())
        return {};

    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = info.applicationName;
    applicationInfo.applicationVersion = info.applicationVersion;
    applicationInfo.pEngineName = info.engineName;
    applicationInfo.engineVersion = info.engineVersion;
    applicationInfo.apiVersion = info.apiVersion;

    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledLayerCount = 0;
    instanceCreateInfo.ppEnabledLayerNames = nullptr;
    instanceCreateInfo.enabledExtensionCount =
        static_cast<std::uint32_t>(info.extensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = info.extensions.data();

#ifndef NDEBUG
    VkDebugUtilsMessengerCreateInfoEXT instanceDebugCreateInfo{};
    instanceDebugCreateInfo.sType =
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    instanceDebugCreateInfo.messageSeverity = info.debugMessageSeverity;
    instanceDebugCreateInfo.messageType = info.debugMessageType;
    instanceDebugCreateInfo.pfnUserCallback = info.debugCallback;
    instanceDebugCreateInfo.pUserData = info.userData;

    instanceCreateInfo.enabledLayerCount =
        static_cast<std::uint32_t>(info.validationLayers.size());
    instanceCreateInfo.ppEnabledLayerNames = info.validationLayers.data();
    instanceCreateInfo.pNext = &instanceDebugCreateInfo;
#endif

    VkInstance instance{ VK_NULL_HANDLE };
    VKCHECK(
        vkCreateInstance(
            &instanceCreateInfo,
            allocationCallbacks.get(),
            &instance),
        "Instance failed to create.");

#ifndef NDEBUG
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    debugCreateInfo.sType =
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity = info.debugMessageSeverity;
    debugCreateInfo.messageType = info.debugMessageType;
    debugCreateInfo.pfnUserCallback = info.debugCallback;
    debugCreateInfo.pUserData = info.userData;

    VkDebugUtilsMessengerEXT debugUtilsMessenger{ VK_NULL_HANDLE };
    VKCHECK(
        createDebugUtilsMessengerEXT(
            instance,
            &debugCreateInfo,
            allocationCallbacks.get(),
            &debugUtilsMessenger),
        "Failed to create debug messenger.");
#endif

    return Instance{
        .handle = instance,
        .debugUtilsMessenger = debugUtilsMessenger,
    };
}

bool InstanceBuilder::Validate()
{
    bool isValid{ true };

#ifndef NDEBUG
    if (info.validationLayers.empty())
    {
        LOG_WARNING("Validation layers not set. Used default.");
        info.validationLayers = VALIDATION_LAYERS;
    }

    std::uint32_t layerCount{ 0 };
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : info.validationLayers)
    {
        bool layerFound{ false };

        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            LOG_ERROR("{} validation layer not supported", layerName);
            isValid = false;
        }
    }

    if (info.debugCallback == nullptr)
    {
        LOG_WARNING("Debug callback not set. Used default.");
        info.debugCallback = defaultDebugCallback;
    }

    if (info.debugMessageType == 0)
    {
        LOG_WARNING("Debug Message Type not set. Used default.");
        info.debugMessageType = DEBUG_MESSAGE.MESSAGE_TYPE;
    }

    if (info.debugMessageSeverity == 0)
    {
        LOG_WARNING("Debug Message Severity not set. Used default.");
        info.debugMessageSeverity = DEBUG_MESSAGE.MESSAGE_SEVERITY;
    }
#endif

    if (info.extensions.empty())
    {
        LOG_ERROR("Extensions not set");
        isValid = false;
    }

    if (info.applicationName == nullptr)
    {
        LOG_ERROR("Application name not set");
        isValid = false;
    }

    if (info.engineName == nullptr)
    {
        LOG_ERROR("Engine name not set");
        isValid = false;
    }

    if (info.applicationVersion == 0)
    {
        LOG_ERROR("Application version not set");
        isValid = false;
    }

    if (info.engineVersion == 0)
    {
        LOG_ERROR("Engine version not set");
        isValid = false;
    }

    if (info.apiVersion == 0)
    {
        LOG_WARNING("Api version not set");
        info.apiVersion = VK_API_VERSION_1_3;
    }

    return isValid;
}

void InstanceBuilder::SetAppName(const char* name)
{
    info.applicationName = name;
}

void InstanceBuilder::SetEngineName(const char* name)
{
    info.engineName = name;
}

void InstanceBuilder::SetApplicationVersion(std::uint32_t applicationVersion)
{
    info.applicationVersion = applicationVersion;
}

void InstanceBuilder::SetEngineVersion(std::uint32_t engineVersion)
{
    info.engineVersion = engineVersion;
}

void InstanceBuilder::SetApiVersion(std::uint32_t apiVersion)
{
    info.apiVersion = apiVersion;
}

void InstanceBuilder::SetExtensions(const std::vector<const char*>& extensions)
{
    for (const auto& extension : extensions)
    {
        info.extensions.push_back(extension);
    }
}

void InstanceBuilder::SetValidationLayers(
    const std::vector<const char*>& validationLayers)
{
    for (const auto& layer : validationLayers)
    {
        info.validationLayers.push_back(layer);
    }
}

void InstanceBuilder::SetDebugCallback(
    PFN_vkDebugUtilsMessengerCallbackEXT callback)
{
    info.debugCallback = callback;
}

void InstanceBuilder::SetDebugMessageSeverity(
    VkDebugUtilsMessageSeverityFlagsEXT debugMessageSeverity)
{
    info.debugMessageSeverity = debugMessageSeverity;
}

void InstanceBuilder::SetDebugMessageType(
    VkDebugUtilsMessageTypeFlagsEXT debugMessageType)
{
    info.debugMessageType = debugMessageType;
}

void InstanceBuilder::SetUserData(void* userData)
{
    info.userData = userData;
}
}
