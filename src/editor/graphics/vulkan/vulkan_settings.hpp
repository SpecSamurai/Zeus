#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
inline constexpr std::uint32_t APPLICATION_VERSION{ VK_MAKE_VERSION(0, 1, 0) };

inline const std::vector<const char*> DEVICE_EXTENSIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

inline constexpr bool VSYNC{ true };

// We choose the number 2 because we don't want the CPU to get too far ahead of
// the GPU. With 2 frames in flight, the CPU and the GPU can be working on their
// own tasks at the same time. If the CPU finishes early, it will wait till the
// GPU finishes rendering before submitting more work. With 3 or more frames in
// flight, the CPU could get ahead of the GPU, adding frames of latency.
inline constexpr int MAX_FRAMES_IN_FLIGHT{ 2 };

inline constexpr struct FaceCullingConfig
{
    VkCullModeFlags cullMode{ VK_CULL_MODE_NONE };
    VkFrontFace frontFace{ VK_FRONT_FACE_COUNTER_CLOCKWISE };
} CULLING;

inline const std::vector<const char*> VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation",
};

inline constexpr struct DebugMessage
{
    VkDebugUtilsMessageSeverityFlagsEXT MESSAGE_SEVERITY{
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        // VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
    };

    VkDebugUtilsMessageTypeFlagsEXT MESSAGE_TYPE{
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
    };
} DEBUG_MESSAGE;
}
