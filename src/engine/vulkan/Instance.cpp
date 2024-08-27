#include "Instance.hpp"

#include "vulkan_debug.hpp"
#include "vulkan_memory.hpp"

#include <vulkan/vulkan.h>

namespace Zeus
{
void destroyInstance(Instance& instance)
{
#ifndef NDEBUG
    destroyDebugUtilsMessengerEXT(
        instance.handle,
        instance.debugUtilsMessenger,
        allocationCallbacks.get());
#endif

    vkDestroyInstance(instance.handle, allocationCallbacks.get());
}
}
