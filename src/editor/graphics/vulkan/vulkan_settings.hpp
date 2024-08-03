#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
inline constexpr struct InstanceConfig
{
    inline static const char* APPLICATION_NAME = "Zeus";
    inline static const char* ENGINE_NAME = "Zeus Engine";

    inline static constexpr std::uint32_t APPLICATION_VERSION{
        VK_MAKE_VERSION(0, 1, 0)
    };
} INSTANCE;

inline const std::vector<const char*> DEVICE_EXTENSIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

inline constexpr bool VSYNC{ true };

inline constexpr std::uint32_t FRAMES_IN_FLIGHT{ 2 };

inline constexpr struct FaceCullingConfig
{
    VkCullModeFlags cullMode{ VK_CULL_MODE_NONE };
    VkFrontFace frontFace{ VK_FRONT_FACE_COUNTER_CLOCKWISE };
} CULLING;
}
