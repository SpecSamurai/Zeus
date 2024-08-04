#include "vulkan_command_buffer.hpp"

#include "Device.hpp"
#include "vulkan_utils.hpp"

#include <core/logger.hpp>

#include <vulkan/vulkan_core.h>

#include <array>
#include <cstdint>
#include <vector>

namespace Zeus
{
bool createVkCommandPool(
    const VkDevice& device,
    std::uint32_t queueFamilyIndex,
    VkCommandPool& commandPool)
{
    VkCommandPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = queueFamilyIndex;

    VkResult result{
        vkCreateCommandPool(device, &createInfo, nullptr, &commandPool)
    };

    if (result != VK_SUCCESS)
    {
        error("Failed to create command pool. {}", vkResultToString(result));
    }

    return result == VK_SUCCESS;
}

bool allocateVkCommandBuffers(
    const VkDevice& device,
    const VkCommandPool& commandPool,
    std::vector<VkCommandBuffer>& commandBuffers)
{
    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandPool = commandPool;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount =
        static_cast<std::uint32_t>(commandBuffers.size());

    VkResult result{
        vkAllocateCommandBuffers(device, &allocateInfo, commandBuffers.data())
    };

    if (result != VK_SUCCESS)
    {
        error(
            "Failed to allocate command buffers. {}",
            vkResultToString(result));
    }

    return result == VK_SUCCESS;
}

bool beginVkCommandBuffer(VkCommandBuffer& commandBuffer)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkResult result{ vkBeginCommandBuffer(commandBuffer, &beginInfo) };

    if (result != VK_SUCCESS)
    {
        error(
            "Failed to begin recording command buffer. {}",
            vkResultToString(result));
    }

    return result == VK_SUCCESS;
}

bool endVkCommandBuffer(VkCommandBuffer& commandBuffer)
{
    VkResult result{ vkEndCommandBuffer(commandBuffer) };

    if (result != VK_SUCCESS)
    {
        error("Failed to record command buffer. {}", vkResultToString(result));
    }

    return result == VK_SUCCESS;
}

bool beginOneTimeVkCommandBuffer(
    const VkDevice& device,
    const VkCommandPool& commandPool,
    VkCommandBuffer& commandBuffer)
{
    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandPool = commandPool;
    allocateInfo.commandBufferCount = 1;

    vkAllocateCommandBuffers(device, &allocateInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VkResult result{ vkBeginCommandBuffer(commandBuffer, &beginInfo) };

    if (result != VK_SUCCESS)
    {
        error(
            "Failed to begin recording command buffer. {}",
            vkResultToString(result));
    }

    return result == VK_SUCCESS;
}

bool endOneTimeVkCommandBuffer(
    const Device& device,
    const VkCommandPool& commandPool,
    VkCommandBuffer& commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkResult result{
        vkQueueSubmit(device.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE)
    };

    if (result != VK_SUCCESS)
    {
        error("Failed to submit command buffer. {}", vkResultToString(result));
    }

    vkQueueWaitIdle(device.graphicsQueue);

    vkFreeCommandBuffers(device.logicalDevice, commandPool, 1, &commandBuffer);

    return true;
}

void cmdBeginVkRenderPass(
    const VkRenderPass& renderPass,
    const VkExtent2D& extent,
    const VkFramebuffer& framebuffer,
    const std::array<VkClearValue, 2>& clearValues,
    VkCommandBuffer& commandBuffer)
{
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = framebuffer;
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = extent;

    renderPassInfo.clearValueCount =
        static_cast<std::uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(
        commandBuffer,
        &renderPassInfo,
        VK_SUBPASS_CONTENTS_INLINE);
}

void cmdSetVkViewport(const VkExtent2D& extent, VkCommandBuffer& commandBuffer)
{
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
}

void cmdSetVkScissor(const VkExtent2D& extent, VkCommandBuffer& commandBuffer)
{
    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = extent;

    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}
}
