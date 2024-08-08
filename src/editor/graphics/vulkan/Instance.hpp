#pragma once

#include <vulkan/vulkan_core.h>

namespace Zeus
{


struct Instance
{
    VkInstance handle{ VK_NULL_HANDLE };
#ifndef NDEBUG
    VkDebugUtilsMessengerEXT debugUtilsMessenger{ VK_NULL_HANDLE };
#endif
};

void destroyInstance(Instance& instance);
}
