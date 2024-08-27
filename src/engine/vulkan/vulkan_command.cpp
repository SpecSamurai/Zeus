#include "vulkan_command.hpp"

#include "vulkan_debug.hpp"
#include "vulkan_memory.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
VkResult createVkCommandPool(
    const VkDevice& device,
    VkCommandPoolCreateFlagBits createFlags,
    std::uint32_t queueFamilyIndex,
    VkCommandPool& commandPool)
{
    VkCommandPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = createFlags;
    createInfo.queueFamilyIndex = queueFamilyIndex;

    VkResult result{ vkCreateCommandPool(
        device,
        &createInfo,
        allocationCallbacks.get(),
        &commandPool) };

    VKCHECK(result, "Failed to create command pool.");

    return result;
}

VkResult allocateVkCommandBuffers(
    std::vector<VkCommandBuffer>& commandBuffers,
    const VkDevice& device,
    const VkCommandPool& commandPool,
    VkCommandBufferLevel level)
{
    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandPool = commandPool;
    allocateInfo.level = level;
    allocateInfo.commandBufferCount =
        static_cast<std::uint32_t>(commandBuffers.size());

    VkResult result{
        vkAllocateCommandBuffers(device, &allocateInfo, commandBuffers.data())
    };

    VKCHECK(result, "Failed to allocate command buffers.");

    return result;
}

VkResult allocateVkCommandBuffer(
    VkCommandBuffer& commandBuffers,
    const VkDevice& device,
    const VkCommandPool& commandPool,
    VkCommandBufferLevel level)
{
    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandPool = commandPool;
    allocateInfo.level = level;
    allocateInfo.commandBufferCount = 1;

    VkResult result{
        vkAllocateCommandBuffers(device, &allocateInfo, &commandBuffers)
    };

    VKCHECK(result, "Failed to allocate command buffers.");

    return result;
}

VkResult beginVkCommandBuffer(
    VkCommandBuffer& commandBuffer,
    VkCommandBufferUsageFlags flags,
    const VkCommandBufferInheritanceInfo* pInheritanceInfo)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = flags;
    beginInfo.pInheritanceInfo = pInheritanceInfo;

    VkResult result{ vkBeginCommandBuffer(commandBuffer, &beginInfo) };

    VKCHECK(result, "Failed to begin recording command buffer.");

    return result;
}

void cmdBeginVkRenderPass(
    VkCommandBuffer& commandBuffer,
    const VkRenderPass& renderPass,
    const VkExtent2D& extent,
    const VkFramebuffer& framebuffer,
    const std::vector<VkClearValue>& clearValues,
    VkOffset2D offset,
    VkSubpassContents contents)
{
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = framebuffer;
    renderPassInfo.renderArea.offset = offset;
    renderPassInfo.renderArea.extent = extent;

    renderPassInfo.clearValueCount =
        static_cast<std::uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, contents);
}

VkResult cmdVkQueueSubmit(
    VkQueue queue,
    std::uint32_t commandBufferCount,
    const VkCommandBuffer* pCommandBuffers,
    VkFence fence,
    std::uint32_t waitSemaphoreCount,
    const VkSemaphore* pWaitSemaphores,
    const VkPipelineStageFlags* pWaitDstStageMask,
    std::uint32_t signalSemaphoreCount,
    const VkSemaphore* pSignalSemaphores)
{
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    submitInfo.waitSemaphoreCount = waitSemaphoreCount;
    submitInfo.pWaitSemaphores = pWaitSemaphores;
    submitInfo.pWaitDstStageMask = pWaitDstStageMask;

    submitInfo.commandBufferCount = commandBufferCount;
    submitInfo.pCommandBuffers = pCommandBuffers;

    submitInfo.signalSemaphoreCount = signalSemaphoreCount;
    submitInfo.pSignalSemaphores = pSignalSemaphores;

    VkResult result{ vkQueueSubmit(queue, 1, &submitInfo, fence) };

    VKCHECK(result, "Failed to submit command buffer.");

    return result;
}

VkSemaphoreSubmitInfo createVkSemaphoreSubmitInfo(
    VkSemaphore semaphore,
    VkPipelineStageFlags2 stageMask)
{
    VkSemaphoreSubmitInfo info{};
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    info.semaphore = semaphore;

    // value is either the value used to signal semaphore or the value waited on
    // by semaphore, if semaphore is a timeline semaphore. Otherwise it is
    // ignored.
    // value is used for timeline semaphores, which are a special type of
    // semaphore where they work through a counter intead of a binary state.
    info.value = 1;
    info.stageMask = stageMask;

    // deviceIndex is the index of the device within a device group that
    // executes the semaphore wait or signal operation.
    // device index parameter is used for multi-gpu semaphore usage.
    info.deviceIndex = 0;

    return info;
}

VkCommandBufferSubmitInfo createVkCommandBufferSubmitInfo(
    VkCommandBuffer commandBuffer)
{
    VkCommandBufferSubmitInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    info.commandBuffer = commandBuffer;

    // deviceMask is a bitmask indicating which devices in a device group
    // execute the command buffer.
    // A deviceMask of 0 is equivalent to setting all bits corresponding to
    // valid devices in the group to 1
    info.deviceMask = 0;

    return info;
}

VkResult cmdVkQueueSubmit2(
    VkQueue queue,
    VkFence fence,
    std::uint32_t waitSemaphoreInfoCount,
    const VkSemaphoreSubmitInfo* pWaitSemaphoreInfos,
    std::uint32_t commandBufferInfoCount,
    const VkCommandBufferSubmitInfo* pCommandBufferInfos,
    std::uint32_t signalSemaphoreInfoCount,
    const VkSemaphoreSubmitInfo* pSignalSemaphoreInfos)
{
    VkSubmitInfo2 submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;

    submitInfo.waitSemaphoreInfoCount = waitSemaphoreInfoCount;
    submitInfo.pWaitSemaphoreInfos = pWaitSemaphoreInfos;

    submitInfo.commandBufferInfoCount = commandBufferInfoCount;
    submitInfo.pCommandBufferInfos = pCommandBufferInfos;

    submitInfo.signalSemaphoreInfoCount = signalSemaphoreInfoCount;
    submitInfo.pSignalSemaphoreInfos = pSignalSemaphoreInfos;

    VkResult result{ vkQueueSubmit2(queue, 1, &submitInfo, fence) };

    VKCHECK(result, "Failed to submit command buffer.");

    return result;
}

VkResult cmdVkQueuePresentKHR(
    VkQueue presentQueue,
    std::uint32_t waitSemaphoreCount,
    const VkSemaphore* pWaitSemaphores,
    std::uint32_t swapchainCount,
    const VkSwapchainKHR* pSwapchains,
    const std::uint32_t* pImageIndices,
    VkResult* pResults)
{
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = waitSemaphoreCount;
    presentInfo.pWaitSemaphores = pWaitSemaphores;

    presentInfo.swapchainCount = swapchainCount;
    presentInfo.pSwapchains = pSwapchains;

    presentInfo.pImageIndices = pImageIndices;

    // A pointer to an array of at least swapchainCount element.
    // The result of the presenting operation will be stored in each of its
    // elements, for each swap chain respectively.
    // A single value returned by the whole function is the same as the worst
    // result value from all swapchains.
    presentInfo.pResults = pResults;

    VkResult result{ vkQueuePresentKHR(presentQueue, &presentInfo) };

    VKCHECK(result, "Failed to present image.");

    return result;
}

void cmdSetVkViewport(
    VkCommandBuffer& commandBuffer,
    float width,
    float height,
    float x,
    float y,
    float minDepth,
    float maxDepth)
{
    VkViewport viewport{};
    viewport.x = x;
    viewport.y = y;
    viewport.width = width;
    viewport.height = height;
    viewport.minDepth = minDepth;
    viewport.maxDepth = maxDepth;

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
}

void cmdSetVkScissor(
    VkCommandBuffer& commandBuffer,
    VkExtent2D extent,
    VkOffset2D offset)
{
    VkRect2D scissor{};
    scissor.offset = offset;
    scissor.extent = extent;

    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}
}
