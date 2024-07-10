#include "vulkan_command_buffer.hpp"

#include "vulkan_device.hpp"
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
    //  VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT – Indicates that command
    //  buffers, allocated from this pool, may be reset individually. Normally,
    //  without this flag, we can’t rerecord the same command buffer multiple
    //  times. It must be reset first. And, what’s more, command buffers created
    //  from one pool may be reset only all at once. Specifying this flag allows
    //  us to reset command buffers individually, and (even better) it is done
    //  implicitly by calling the vkBeginCommandBuffer() function.

    // VK_COMMAND_POOL_CREATE_TRANSIENT_BIT – This flag tells the driver that
    // command buffers allocated from this pool will be living for a short
    // amount of time, they will be often recorded and reset (re-recorded). This
    // information helps optimize command buffer allocation and perform it more
    // optimally.

    VkCommandPoolCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = VK_NULL_HANDLE,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = queueFamilyIndex,
    };

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
    VkCommandBufferAllocateInfo allocateInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = VK_NULL_HANDLE,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = static_cast<std::uint32_t>(commandBuffers.size()),
    };

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
    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = VK_NULL_HANDLE,
        .flags = 0,
        .pInheritanceInfo = nullptr,
    };

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

// Memory transfer operations are executed using command buffers, just like
// drawing commands. Therefore we must first allocate a temporary command
// buffer. You may wish to create a separate command pool for these kinds of
// short-lived buffers, because the implementation may be able to apply
// memory allocation optimizations. You should use the
// VK_COMMAND_POOL_CREATE_TRANSIENT_BIT flag during command pool generation
// in that case.
VkCommandBuffer beginSingleTimeCommands(
    VkDevice device,
    VkCommandPool commandPool)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    // We're only going to use the command buffer once and wait with
    // returning from the function until the copy operation has finished
    // executing. It's good practice to tell the driver about our intent
    // using VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT.
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void endSingleTimeCommands(
    VulkanDevice device,
    VkCommandBuffer commandBuffer,
    VkCommandPool commandPool)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(device.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(device.graphicsQueue);

    vkFreeCommandBuffers(device.logicalDevice, commandPool, 1, &commandBuffer);
}

void cmdBeginVkRenderPass(
    const VkRenderPass& renderPass,
    const VkExtent2D& extent,
    const VkFramebuffer& framebuffer,
    const std::array<VkClearValue, 1>& clearValues,
    VkCommandBuffer& commandBuffer)
{
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = framebuffer;
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = extent;

    // The last two parameters define the clear values to use for
    // VK_ATTACHMENT_LOAD_OP_CLEAR, which we used as load operation for the
    // color attachment. I've defined the clear color to simply be black
    // with 100% opacity.
    // VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    // renderPassInfo.clearValueCount = 1;
    // renderPassInfo.pClearValues = &clearColor;
    // std::array<VkClearValue, 1> clearValues{};
    // clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
    // clearValues[1].depthStencil = { 1.0f, 0 };

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
