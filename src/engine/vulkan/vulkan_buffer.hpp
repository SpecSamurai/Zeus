#pragma once

#include "vulkan_memory.hpp"

#include <vulkan/vulkan.h>

namespace Zeus
{
struct Buffer
{
    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo info;
};

VkResult createBuffer(
    VmaAllocator allocator,
    Buffer& buffer,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags memoryPropertyFlags,
    bool map = false);

void cmdCopyBuffer(
    VkCommandBuffer commandBuffer,
    VkBuffer srcBuffer,
    VkBuffer dstBuffer,
    VkDeviceSize srcOffset,
    VkDeviceSize dstOffset,
    VkDeviceSize size);

VkDeviceAddress getBufferDeviceAddress(VkDevice device, VkBuffer& buffer);
}
