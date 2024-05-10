#pragma once

#include <vulkan/vulkan_core.h>
namespace Zeus
{
bool createVkSemaphore(const VkDevice& device, VkSemaphore& semaphore);
bool createVkFence(const VkDevice& device, bool signaled, VkFence& fence);
}
