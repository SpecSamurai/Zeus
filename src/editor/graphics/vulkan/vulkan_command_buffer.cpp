#include "vulkan_command_buffer.hpp"

#include "vulkan_utils.hpp"

#include <core/logger.hpp>

#include <vulkan/vulkan_core.h>

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
}
