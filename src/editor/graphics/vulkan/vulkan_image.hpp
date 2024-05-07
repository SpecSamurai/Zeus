#pragma once

#include <vulkan/vulkan_core.h>

namespace Zeus
{
bool createVkImageView(
    const VkDevice& device,
    const VkImage& image,
    const VkFormat format,
    const VkImageAspectFlags aspectFlags,
    uint32_t mipLevels,
    VkImageView& imageView);
}
