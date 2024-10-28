#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
VkResult createVkFramebuffer(
    VkFramebuffer* framebuffer,
    VkDevice device,
    const std::vector<VkImageView>& attachments,
    VkRenderPass renderPass,
    const VkExtent2D& extent,
    std::uint32_t layers = 1);
}
