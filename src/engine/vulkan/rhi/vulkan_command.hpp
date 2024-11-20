#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>

namespace Zeus
{
[[deprecated("Use vkQueueSubmit2 instead.")]]
VkResult cmdVkQueueSubmit(
    VkQueue queue,
    std::uint32_t commandBufferCount,
    const VkCommandBuffer* pCommandBuffers,
    VkFence fence = VK_NULL_HANDLE,
    std::uint32_t waitSemaphoreCount = 0,
    const VkSemaphore* pWaitSemaphores = nullptr,
    const VkPipelineStageFlags* pWaitDstStageMask = nullptr,
    std::uint32_t signalSemaphoreCount = 0,
    const VkSemaphore* pSignalSemaphores = nullptr);

VkSemaphoreSubmitInfo createVkSemaphoreSubmitInfo(
    VkSemaphore semaphore,
    VkPipelineStageFlags2 stageMask);

VkCommandBufferSubmitInfo createVkCommandBufferSubmitInfo(
    VkCommandBuffer commandBuffer);
}
