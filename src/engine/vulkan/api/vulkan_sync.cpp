#include "vulkan_sync.hpp"

#include "vulkan_debug.hpp"
#include "vulkan_memory.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>

namespace Zeus
{
VkResult createVkSemaphore(
    const VkDevice& device,
    VkSemaphore& semaphore,
    bool isTimeline)
{
    VkSemaphoreTypeCreateInfo typeCreateInfo{};
    typeCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    typeCreateInfo.pNext = nullptr;
    typeCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    typeCreateInfo.initialValue = 0;

    VkSemaphoreCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    createInfo.pNext = isTimeline ? &typeCreateInfo : nullptr;
    createInfo.flags = 0;

    VkResult result{ vkCreateSemaphore(
        device,
        &createInfo,
        allocationCallbacks.get(),
        &semaphore) };

    VKCHECK(result, "Failed to create Semaphore.");

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

    VKCHECK(result, "Failed to create Fence.");

    return result;
}

VkResult waitVkSemaphores(
    const VkDevice& device,
    VkSemaphore& semaphore,
    std::uint64_t value,
    std::uint64_t timeout_ns)
{
    VkSemaphoreWaitInfo waitInfo{};
    waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
    waitInfo.pNext = nullptr;
    waitInfo.flags = 0;
    waitInfo.semaphoreCount = 1;
    waitInfo.pSemaphores = &semaphore;
    waitInfo.pValues = &value;

    VkResult result{ vkWaitSemaphores(device, &waitInfo, timeout_ns) };

    VKCHECK(result, "Failed to wait for Semaphore.");

    return result;
}

VkResult signalVkSemaphores(
    const VkDevice& device,
    VkSemaphore& semaphore,
    std::uint64_t value)
{
    VkSemaphoreSignalInfo signalInfo{};
    signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
    signalInfo.pNext = nullptr;
    signalInfo.semaphore = semaphore;
    signalInfo.value = value;

    VkResult result{ vkSignalSemaphore(device, &signalInfo) };

    VKCHECK(result, "Failed to signal Semaphore.");

    return result;
}
}
