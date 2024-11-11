#pragma once

#include <vulkan/vulkan.h>

namespace Zeus
{
VkResult createVkSurfaceKHR(
    VkInstance instance,
    void* windowHandle,
    VkSurfaceKHR* surface);
}
