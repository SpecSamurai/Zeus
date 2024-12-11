#include "vulkan_command.hpp"

#include "vulkan_debug.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>

namespace Zeus
{
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
    return VkSemaphoreSubmitInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .pNext = nullptr,
        .semaphore = semaphore,
        // value is either the value used to signal semaphore or the value
        // waited on by semaphore, if semaphore is a timeline semaphore.
        // Otherwise it is ignored.
        // value is used for timeline semaphores, which are a special type of
        // semaphore where they work through a counter intead of a binary state.
        .value = 1,
        .stageMask = stageMask,
        // deviceIndex is the index of the device within a device group that
        // executes the semaphore wait or signal operation.
        // device index parameter is used for multi-gpu semaphore usage.
        .deviceIndex = 0,
    };
}

VkCommandBufferSubmitInfo createVkCommandBufferSubmitInfo(
    VkCommandBuffer commandBuffer)
{
    // deviceMask is a bitmask indicating which devices in a device group
    // execute the command buffer.
    // A deviceMask of 0 is equivalent to setting all bits corresponding to
    // valid devices in the group to 1
    return VkCommandBufferSubmitInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .pNext = nullptr,
        .commandBuffer = commandBuffer,
        .deviceMask = 0,
    };
}
}
