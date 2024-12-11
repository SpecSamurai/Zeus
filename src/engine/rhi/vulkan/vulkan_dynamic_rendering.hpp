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
}
