#include "vulkan_buffer.hpp"

#include <vulkan/vulkan.h>

namespace Zeus
{
void cmdCopyBuffer(
    VkCommandBuffer commandBuffer,
    VkBuffer srcBuffer,
    VkBuffer dstBuffer,
    VkDeviceSize srcOffset,
    VkDeviceSize dstOffset,
    VkDeviceSize size)
{
    VkBufferCopy2 vertexBufferCopy{};
    vertexBufferCopy.sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2;
    vertexBufferCopy.srcOffset = srcOffset;
    vertexBufferCopy.dstOffset = dstOffset;
    vertexBufferCopy.size = size;

    VkCopyBufferInfo2 vertexBufferCopyInfo{};
    vertexBufferCopyInfo.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2;
    vertexBufferCopyInfo.pNext = nullptr;
    vertexBufferCopyInfo.srcBuffer = srcBuffer;
    vertexBufferCopyInfo.dstBuffer = dstBuffer;
    vertexBufferCopyInfo.regionCount = 1;
    vertexBufferCopyInfo.pRegions = &vertexBufferCopy;

    vkCmdCopyBuffer2(commandBuffer, &vertexBufferCopyInfo);
}
}
