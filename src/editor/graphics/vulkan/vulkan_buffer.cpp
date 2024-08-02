#include "vulkan_buffer.hpp"

#include "vulkan_command_buffer.hpp"
#include "vulkan_device.hpp"
#include "vulkan_utils.hpp"

#include <core/logger.hpp>

#include <vulkan/vulkan_core.h>

namespace Zeus
{
bool createVkBuffer(
    const VkDevice& device,
    const VkPhysicalDevice& physicalDevice,
    const BufferConfig& config,
    VkBuffer& buffer,
    VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = config.size;
    bufferCreateInfo.usage = config.usage;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult createResult{
        vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffer)
    };

    if (createResult != VK_SUCCESS)
    {
        error("Failed to create buffer. {}", vkResultToString(createResult));
        return false;
    }

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = findMemoryType(
        physicalDevice,
        memoryRequirements.memoryTypeBits,
        config.properties);

    VkResult allocateResult{
        vkAllocateMemory(device, &allocateInfo, nullptr, &bufferMemory)
    };

    if (allocateResult != VK_SUCCESS)
    {
        error(
            "Failed to allocate buffer memory. {}",
            vkResultToString(allocateResult));

        return false;
    }

    // fourth parameter is the offset within the region of memory
    // offset is required to be divisible by memRequirements.alignment.
    VkResult bindResult{ vkBindBufferMemory(device, buffer, bufferMemory, 0) };

    if (bindResult != VK_SUCCESS)
    {
        error("Failed to bind buffer memory. {}", vkResultToString(bindResult));
        return false;
    }

    return true;
}

void cmdCopyBuffer(
    const VulkanDevice& device,
    const VkCommandPool& commandPool,
    const VkBuffer& srcBuffer,
    const VkBuffer& dstBuffer,
    const VkDeviceSize& size)
{
    VkCommandBuffer commandBuffer{
        beginSingleTimeCommands(device.logicalDevice, commandPool)
    };

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;

    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(device, commandBuffer, commandPool);
}
}
