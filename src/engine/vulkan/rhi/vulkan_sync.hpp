#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>

namespace Zeus
{
VkResult createVkSemaphore(
    VkDevice device,
    VkSemaphore* semaphore,
    bool isTimeline = false);

VkResult createVkFence(VkDevice device, bool signaled, VkFence* fence);

VkResult waitVkSemaphores(
    VkDevice device,
    VkSemaphore* semaphore,
    std::uint64_t value,
    std::uint64_t timeout_ns);

VkResult signalVkSemaphores(
    VkDevice device,
    VkSemaphore semaphore,
    std::uint64_t value);
}
