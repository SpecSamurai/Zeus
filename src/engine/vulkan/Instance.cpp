#include "Instance.hpp"

#include "vulkan_debug.hpp"

#include <vulkan/vulkan.h>

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
