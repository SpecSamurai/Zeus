#include "Instance.hpp"

#include "MemoryAllocator.hpp"
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
        MemoryAllocator::pAllocator.get());
#endif

    vkDestroyInstance(instance.handle, MemoryAllocator::pAllocator.get());
}
}
