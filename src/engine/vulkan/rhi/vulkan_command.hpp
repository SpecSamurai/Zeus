#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
VkResult beginVkCommandBuffer(
    VkCommandBuffer commandBuffer,
    VkCommandBufferUsageFlags flags = 0,
    const VkCommandBufferInheritanceInfo* pInheritanceInfo = nullptr);

void cmdBeginVkRenderPass(
    VkCommandBuffer commandBuffer,
    VkRenderPass renderPass,
    const VkExtent2D& extent,
    VkFramebuffer framebuffer,
    const std::vector<VkClearValue>& clearValues,
    VkOffset2D offset = { .x = 0, .y = 0 },
    VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE);

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

VkResult cmdVkQueueSubmit2(
    VkQueue queue,
    VkFence fence,
    std::uint32_t waitSemaphoreInfoCount,
    const VkSemaphoreSubmitInfo* pWaitSemaphoreInfos,
    std::uint32_t commandBufferInfoCount,
    const VkCommandBufferSubmitInfo* pCommandBufferInfos,
    std::uint32_t signalSemaphoreInfoCount,
    const VkSemaphoreSubmitInfo* pSignalSemaphoreInfos);

VkResult cmdVkQueuePresentKHR(
    VkQueue presentQueue,
    std::uint32_t waitSemaphoreCount,
    const VkSemaphore* pWaitSemaphores,
    std::uint32_t swapchainCount,
    const VkSwapchainKHR* pSwapchains,
    const std::uint32_t* pImageIndices,
    VkResult* pResults = nullptr);

void cmdSetVkViewport(
    VkCommandBuffer commandBuffer,
    float width,
    float height,
    float x = 0.0f,
    float y = 0.0f,
    float minDepth = 0.0f,
    float maxDepth = 1.0f);

void cmdSetVkScissor(
    VkCommandBuffer commandBuffer,
    VkExtent2D extent,
    VkOffset2D offset = { .x = 0, .y = 0 });
}
