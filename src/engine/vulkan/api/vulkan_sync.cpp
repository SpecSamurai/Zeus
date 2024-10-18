#include "vulkan_sync.hpp"

#include "vulkan_debug.hpp"
#include "vulkan_memory.hpp"

#include <vulkan/vulkan.h>

namespace Zeus
{
VkResult createVkSemaphore(const VkDevice& device, VkSemaphore& semaphore)
{
    VkSemaphoreCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkResult result{ vkCreateSemaphore(
        device,
        &createInfo,
        allocationCallbacks.get(),
        &semaphore) };

    VKCHECK(result, "Failed to create a Semaphore.");

    return result;
}

VkResult createVkFence(const VkDevice& device, bool signaled, VkFence& fence)
{
    VkFenceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    if (signaled)
    {
        createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    }

    VkResult result{
        vkCreateFence(device, &createInfo, allocationCallbacks.get(), &fence)
    };

    VKCHECK(result, "Failed to create a Fence.");

    return result;
}
}
