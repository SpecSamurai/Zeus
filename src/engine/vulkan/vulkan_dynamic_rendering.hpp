#pragma once

#include <vulkan/vulkan.h>

namespace Zeus
{
VkRenderingAttachmentInfo createDepthAttachmentInfo(
    VkImageView imageView,
    VkImageLayout imageLayout);

VkRenderingAttachmentInfo createColorAttachmentInfo(
    VkImageView imageView,
    VkImageLayout imageLayout,
    const VkClearValue* clearValue = nullptr);

void cmdBeginRendering(
    VkCommandBuffer commandBuffer,
    VkExtent2D renderExtent, // VkRect2D renderArea,
    uint32_t colorAttachmentCount,
    const VkRenderingAttachmentInfo* pColorAttachments,
    const VkRenderingAttachmentInfo* pDepthAttachment = nullptr,
    const VkRenderingAttachmentInfo* pStencilAttachment = nullptr,
    VkRenderingFlags flags = 0,
    uint32_t layerCount = 1,
    uint32_t viewMask = 0);
}
