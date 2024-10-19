#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>

namespace Zeus
{
VkResult createVkSemaphore(
    const VkDevice& device,
    VkSemaphore& semaphore,
    bool isTimeline = false);

VkResult createVkFence(const VkDevice& device, bool signaled, VkFence& fence);

VkResult waitVkSemaphores(
    const VkDevice& device,
    VkSemaphore& semaphore,
    std::uint64_t value,
    std::uint64_t timeout_ns);

VkResult signalVkSemaphores(
    const VkDevice& device,
    VkSemaphore& semaphore,
    std::uint64_t value);
}
