#pragma once

#include <vulkan/Device.hpp>

#include <vulkan/vulkan_core.h>

namespace Zeus
{
struct BufferConfig
{
    VkDeviceSize size;
    VkBufferUsageFlags usage;
    VkMemoryPropertyFlags properties;
};

bool createVkBuffer(
    const VkDevice& device,
    const VkPhysicalDevice& physicalDevice,
    const BufferConfig& config,
    VkBuffer& buffer,
    VkDeviceMemory& bufferMemory);

void cmdCopyBuffer(
    const Device& device,
    const VkCommandPool& commandPool,
    const VkBuffer& srcBuffer,
    const VkBuffer& dstBuffer,
    const VkDeviceSize& size);
}
