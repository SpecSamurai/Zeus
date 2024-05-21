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
    VkCommandPoolCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = queueFamilyIndex,
    };

    VkResult result{
        vkCreateCommandPool(device, &createInfo, nullptr, &commandPool)
    };

    if (result != VK_SUCCESS)
    {
        error("Failed to create command pool", vkResultToString(result));
    }

    return result == VK_SUCCESS;
}

bool allocateVkCommandBuffers(
    const VkDevice& device,
    const VkCommandPool& commandPool,
    std::vector<VkCommandBuffer>& commandBuffers)
{
    VkCommandBufferAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = static_cast<std::uint32_t>(commandBuffers.size()),
    };

    VkResult result{
        vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data())
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
