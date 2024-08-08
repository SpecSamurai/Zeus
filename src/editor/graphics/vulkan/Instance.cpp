#include "Instance.hpp"

#include <vulkan/vulkan_debug.hpp>
#include <vulkan/vulkan_utils.hpp>

#include <core/logger.hpp>

namespace Zeus
{


void destroyInstance(Instance& instance)
{
#ifndef NDEBUG
    destroyDebugUtilsMessengerEXT(
        instance.handle,
        instance.debugUtilsMessenger,
        nullptr);
#endif

    vkDestroyInstance(instance.handle, nullptr);
}
}
