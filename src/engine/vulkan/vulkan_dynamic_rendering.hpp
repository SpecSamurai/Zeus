#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>

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
    const VkExtent2D& renderExtent,
    std::uint32_t colorAttachmentCount,
    const VkRenderingAttachmentInfo* pColorAttachments,
    const VkRenderingAttachmentInfo* pDepthAttachment = nullptr,
    const VkRenderingAttachmentInfo* pStencilAttachment = nullptr,
    VkRenderingFlags flags = 0,
    std::uint32_t layerCount = 1,
    std::uint32_t viewMask = 0);
}
