#include "vulkan_sync.hpp"

#include "vulkan_utils.hpp"

#include <core/logger.hpp>

#include <vulkan/vulkan_core.h>

namespace Zeus
{
bool createVkSemaphore(const VkDevice& device, VkSemaphore& semaphore)
{
    VkSemaphoreCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    VkResult result{
        vkCreateSemaphore(device, &createInfo, nullptr, &semaphore)
    };
    if (result != VK_SUCCESS)
    {
        error("Failed to create a Semaphore. {}", vkResultToString(result));
    }

    return result == VK_SUCCESS;
}

bool createVkFence(const VkDevice& device, bool signaled, VkFence& fence)
{
    VkFenceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    };

    if (signaled)
    {
        createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    }

    VkResult result{ vkCreateFence(device, &createInfo, nullptr, &fence) };
    if (result != VK_SUCCESS)
    {
        error("Failed to create a Fence. {}", vkResultToString(result));
    }

    return result == VK_SUCCESS;
}
}
