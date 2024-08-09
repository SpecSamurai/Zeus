#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
VkResult createVkCommandPool(
    const VkDevice& device,
    VkCommandPoolCreateFlagBits createFlags,
    std::uint32_t queueFamilyIndex,
    VkCommandPool& commandPool);

VkResult allocateVkCommandBuffers(
    std::vector<VkCommandBuffer>& commandBuffers,
    const VkDevice& device,
    const VkCommandPool& commandPool,
    VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

VkResult allocateVkCommandBuffer(
    VkCommandBuffer& commandBuffers,
    const VkDevice& device,
    const VkCommandPool& commandPool,
    VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

VkResult beginVkCommandBuffer(
    VkCommandBuffer& commandBuffer,
    VkCommandBufferUsageFlags flags = 0,
    const VkCommandBufferInheritanceInfo* pInheritanceInfo = nullptr);

VkResult endVkCommandBuffer(VkCommandBuffer& commandBuffer);

// Recommended use with VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
bool beginOneTimeVkCommandBuffer(
    VkCommandBuffer& commandBuffer,
    const VkDevice& device,
    const VkCommandPool& commandPool);

void endOneTimeVkCommandBuffer(
    VkCommandBuffer& commandBuffer,
    const VkDevice& device,
    const VkQueue& queue,
    const VkCommandPool& commandPool);

void cmdBeginVkRenderPass(
    VkCommandBuffer& commandBuffer,
    const VkRenderPass& renderPass,
    const VkExtent2D& extent,
    const VkFramebuffer& framebuffer,
    const std::vector<VkClearValue>& clearValues,
    VkOffset2D offset = { 0, 0 });

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

VkResult cmdVkQueuePresentKHR(
    VkQueue presentQueue,
    std::uint32_t waitSemaphoreCount,
    const VkSemaphore* pWaitSemaphores,
    std::uint32_t swapchainCount,
    const VkSwapchainKHR* pSwapchains,
    const std::uint32_t* pImageIndices,
    VkResult* pResults = nullptr);

void cmdSetVkViewport(
    VkCommandBuffer& commandBuffer,
    float width,
    float height,
    float x = 0.0f,
    float y = 0.0f,
    float minDepth = 0.0f,
    float maxDepth = 1.0f);

void cmdSetVkScissor(
    VkCommandBuffer& commandBuffer,
    VkExtent2D extent,
    VkOffset2D offset = { 0, 0 });
}
