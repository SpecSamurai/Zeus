#include "InstanceBuilder.hpp"

#include "MemoryAllocator.hpp"
#include "core/logger.hpp"
#include "vulkan_debug.hpp"
#include "vulkan_settings.hpp"
#include "vulkan_utils.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <optional>
#include <vector>

namespace Zeus
{
std::optional<Instance> InstanceBuilder::build()
{
    if (!validate())
        return std::nullopt;

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

    Instance instance{};

    VkResult instanceResult{ vkCreateInstance(
        &instanceCreateInfo,
        MemoryAllocator::pAllocator,
        &instance.handle) };

    VKCHECK(instanceResult, "Instance failed to create.");

#ifndef NDEBUG
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    debugCreateInfo.sType =
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity = info.debugMessageSeverity;
    debugCreateInfo.messageType = info.debugMessageType;
    debugCreateInfo.pfnUserCallback = info.debugCallback;
    debugCreateInfo.pUserData = info.userData;

    VkResult debugResult{ createDebugUtilsMessengerEXT(
        instance.handle,
        &debugCreateInfo,
        nullptr,
        &instance.debugUtilsMessenger) };

    VKCHECK(debugResult, "Failed to create debug messenger.");
#endif

    return instance;
}

bool InstanceBuilder::validate()
{
    bool isValid{ true };

#ifndef NDEBUG
    if (info.validationLayers.empty())
    {
        warning("Validation layers not set. Used default.");
        info.validationLayers = INSTANCE_DEFAULT.VALIDATION_LAYERS;
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
            error("{} validation layer not supported", layerName);
            isValid = false;
        }
    }

    if (info.debugCallback == nullptr)
    {
        warning("Debug callback not set. Used default.");
        info.debugCallback = defaultDebugCallback;
    }

    if (info.debugMessageType == 0)
    {
        warning("Debug Message Type not set. Used default.");
        info.debugMessageType = INSTANCE_DEFAULT.DEBUG_MESSAGE.MESSAGE_TYPE;
    }

    if (info.debugMessageSeverity == 0)
    {
        warning("Debug Message Severity not set. Used default.");
        info.debugMessageSeverity =
            INSTANCE_DEFAULT.DEBUG_MESSAGE.MESSAGE_SEVERITY;
    }
#endif

    if (info.extensions.empty())
    {
        error("Extensions not set");
        isValid = false;
    }

    if (info.applicationName == nullptr)
    {
        error("Application name not set");
        isValid = false;
    }

    if (info.engineName == nullptr)
    {
        error("Engine name not set");
        isValid = false;
    }

    if (info.applicationVersion == 0)
    {
        error("Application version not set");
        isValid = false;
    }

    if (info.engineVersion == 0)
    {
        error("Engine version not set");
        isValid = false;
    }

    if (info.apiVersion == 0)
    {
        warning("Api version not set");
        info.apiVersion = INSTANCE_DEFAULT.API_VERSION;
    }

    return isValid;
}

InstanceBuilder& InstanceBuilder::setAppName(const char* name)
{
    info.applicationName = name;
    return *this;
}

InstanceBuilder& InstanceBuilder::setEngineName(const char* name)
{
    info.engineName = name;
    return *this;
}

InstanceBuilder& InstanceBuilder::setApplicationVersion(
    std::uint32_t applicationVersion)
{
    info.applicationVersion = applicationVersion;
    return *this;
}

InstanceBuilder& InstanceBuilder::setEngineVersion(std::uint32_t engineVersion)
{
    info.engineVersion = engineVersion;
    return *this;
}

InstanceBuilder& InstanceBuilder::setApiVersion(std::uint32_t apiVersion)
{
    info.apiVersion = apiVersion;
    return *this;
}

InstanceBuilder& InstanceBuilder::setExtensions(
    const std::vector<const char*>& extensions)
{
    for (const auto& extension : extensions)
    {
        info.extensions.push_back(extension);
    }
    return *this;
}

InstanceBuilder& InstanceBuilder::setValidationLayers(
    const std::vector<const char*>& validationLayers)
{
    for (const auto& layer : validationLayers)
    {
        info.validationLayers.push_back(layer);
    }
    return *this;
}

InstanceBuilder& InstanceBuilder::setDebugCallback(
    PFN_vkDebugUtilsMessengerCallbackEXT callback)
{
    info.debugCallback = callback;
    return *this;
}

InstanceBuilder& InstanceBuilder::setDebugMessageSeverity(
    VkDebugUtilsMessageSeverityFlagsEXT debugMessageSeverity)
{
    info.debugMessageSeverity = debugMessageSeverity;
    return *this;
}

InstanceBuilder& InstanceBuilder::setDebugMessageType(
    VkDebugUtilsMessageTypeFlagsEXT debugMessageType)
{
    info.debugMessageType = debugMessageType;
    return *this;
}

InstanceBuilder& InstanceBuilder::setUserData(void* userData)
{
    info.userData = userData;
    return *this;
}
}
