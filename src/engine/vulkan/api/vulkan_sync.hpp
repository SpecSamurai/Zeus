#pragma once

#include <vulkan/vulkan.h>

namespace Zeus
{
VkResult createVkSemaphore(const VkDevice& device, VkSemaphore& semaphore);
VkResult createVkFence(const VkDevice& device, bool signaled, VkFence& fence);
}
