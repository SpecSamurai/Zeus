#pragma once

#include <vulkan/vulkan.h>

namespace Zeus
{
void cmdCopyBuffer(
    VkCommandBuffer commandBuffer,
    VkBuffer srcBuffer,
    VkBuffer dstBuffer,
    VkDeviceSize srcOffset,
    VkDeviceSize dstOffset,
    VkDeviceSize size);
}
