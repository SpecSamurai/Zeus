#define GLFW_INCLUDE_VULKAN

#include "logger.hpp"
#include "vulkan.utils.hpp"

#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cstdint>
#include <vector>

namespace Zeus
{
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation",
};

#ifdef NDEBUG
const bool ENABLE_VALIDATION_LAYERS = false;
#else
const bool ENABLE_VALIDATION_LAYERS = true;
#endif

class VulkanInstance
{
public:
    VkInstance instance{VK_NULL_HANDLE};
    VkDebugUtilsMessengerEXT debugMessenger{VK_NULL_HANDLE};

    void CreateInstance(const char* _title)
    {
        if (ENABLE_VALIDATION_LAYERS &&
            !checkValidationLayerSupport(validationLayers))
        {
            critical("Validation layers requested, but not available");
            assert(false);
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = _title;
        appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
        appInfo.pEngineName = "Zeus Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount =
            static_cast<std::uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        // The debugCreateInfo variable is placed outside the if statement to
        // ensure that it is not destroyed before the vkCreateInstance call.
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (ENABLE_VALIDATION_LAYERS)
        {
            createInfo.enabledLayerCount =
                static_cast<std::uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = &debugCreateInfo;
        }
        else
        {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
        {
            critical("Failed to create instance");
            assert(false);
        }
    }

    // Tells Vulkan driver which global extensions and validation layers we want
    // to
    // use. Global here means that they apply to the entire program and not a
    // specific device
    bool checkValidationLayerSupport(
        const std::vector<const char*>& validationLayers)
    {
        std::uint32_t layerCount{0};
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers)
        {
            bool layerFound{false};

            for (const auto& layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    debug("{} layer supported", layerProperties.layerName);
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }

        return true;
    }

    // get required global extensions
    std::vector<const char*> getRequiredExtensions()
    {
        std::uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(
            nullptr,
            &extensionCount,
            nullptr);

        std::vector<VkExtensionProperties> Aextensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(
            nullptr,
            &extensionCount,
            Aextensions.data());

        debug("Supported global extensions:");
        for (const auto& extension : Aextensions)
        {
            debug("> {}", extension.extensionName);
        }

        std::uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(
            glfwExtensions,
            glfwExtensions + glfwExtensionCount);

        if (ENABLE_VALIDATION_LAYERS)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    void populateDebugMessengerCreateInfo(
        VkDebugUtilsMessengerCreateInfoEXT& createInfo)
    {
        createInfo = {};
        createInfo.sType =
            VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            // VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        [[maybe_unused]] void* pUserData)
    {
        switch (messageSeverity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            info("{}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            warning("{}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            error("{}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
            break;
        }

        return VK_FALSE;
    }

    void SetupDebugMessenger()
    {
        if (!ENABLE_VALIDATION_LAYERS)
            return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        if (createDebugUtilsMessengerEXT(
                instance,
                &createInfo,
                nullptr,
                &debugMessenger) != VK_SUCCESS)
        {
            critical("Failed to set up debug messenger");
            assert(false);
        }
    }
};
}
