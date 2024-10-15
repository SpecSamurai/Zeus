#pragma once

#include <vulkan/vulkan.h>

namespace Zeus
{
VkResult createVkSurfaceKHR(
    const VkInstance& instance,
    void* windowHandle,
    VkSurfaceKHR& surface);
}
