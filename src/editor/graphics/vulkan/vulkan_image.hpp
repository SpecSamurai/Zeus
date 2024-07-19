#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>

namespace Zeus
{
bool createVkImageView(
    const VkDevice& device,
    const VkImage& image,
    const VkFormat format,
    const VkImageAspectFlags aspectFlags,
    const std::uint32_t mipLevels,
    VkImageView& imageView);
}
