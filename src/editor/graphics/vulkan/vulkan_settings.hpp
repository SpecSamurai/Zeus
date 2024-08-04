#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
inline constexpr bool VSYNC{ true };

inline constexpr std::uint32_t FRAMES_IN_FLIGHT{ 2 };

inline constexpr struct FaceCullingConfig
{
    VkCullModeFlags cullMode{ VK_CULL_MODE_NONE };
    VkFrontFace frontFace{ VK_FRONT_FACE_COUNTER_CLOCKWISE };
} CULLING;

inline constexpr struct VulkanInstanceConfig
{
    inline static const char* APPLICATION_NAME = "Zeus";
    inline static const char* ENGINE_NAME = "Zeus Engine";

    inline static constexpr std::uint32_t APPLICATION_VERSION{
        VK_MAKE_VERSION(0, 1, 0)
    };

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

    inline static const std::vector<const char*> DEVICE_EXTENSIONS = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

} INSTANCE_DEFAULT;
}
