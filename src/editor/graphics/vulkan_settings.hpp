#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Zeus
{
#ifdef NDEBUG
inline constexpr bool ENABLE_VALIDATION_LAYERS = false;
#else
inline constexpr bool ENABLE_VALIDATION_LAYERS = true;
#endif

inline const std::vector<const char*> VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation",
};

inline const std::vector<const char*> DEVICE_EXTENSIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};
}
