#pragma once

#include "Definitions.hpp"
#include "DeletionQueue.hpp"
#include "Device.hpp"
#include "rhi/vulkan_memory.hpp"
#include "window/Window.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>

namespace Zeus
{
class VkContext
{
public:
    static void Init(const Window& window);
    static void Destroy();

    static VkInstance GetInstance();
    static Device& GetDevice();
    static VkDevice GetLogicalDevice();
    static DeletionQueue& GetDeletionQueue();
    static VkSurfaceKHR GetSurface();
    static VmaAllocator GetAllocator();

    static VkQueue GetQueue(QueueType type);
    static std::uint32_t GetQueueFamily(QueueType type);

    static void SetDebugName(
        VkObjectType objectType,
        void* handle,
        const char* name);

private:
    static VkInstance s_instance;
    static VkDebugUtilsMessengerEXT s_debugUtilsMessenger;
    static VkSurfaceKHR s_surface;
    static Device s_device;
    static VmaAllocator s_allocator;
};
}
