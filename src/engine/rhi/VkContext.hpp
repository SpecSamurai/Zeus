#pragma once

#include "Definitions.hpp"
#include "DeletionQueue.hpp"
#include "Device.hpp"
#include "vulkan/vulkan_memory.hpp"
#include "window/Window.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <string_view>

namespace Zeus
{
class VkContext
{
public:
    static void Initialize(const Window& window);
    static void Destroy();

    static VkInstance Instance();
    static Device& Device();
    static VkDevice LogicalDevice();
    static DeletionQueue& DeletionQueue();
    static VkSurfaceKHR Surface();
    static VmaAllocator Allocator();

    static VkQueue GetQueue(QueueType type);
    static std::uint32_t GetQueueFamily(QueueType type);

    static void SetDebugName(
        VkObjectType objectType,
        void* handle,
        std::string_view name);

private:
    static VkInstance s_instance;
    static VkDebugUtilsMessengerEXT s_debugUtilsMessenger;
    static VkSurfaceKHR s_surface;
    static class Device s_device;
    static VmaAllocator s_allocator;
    static class DeletionQueue s_deletionQueue;
};
}
