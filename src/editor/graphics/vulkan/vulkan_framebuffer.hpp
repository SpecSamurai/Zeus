#pragma once

#include <vulkan/vulkan_core.h>

#include <vector>

namespace Zeus
{
bool createVkFramebuffer(
    const VkDevice& device,
    const std::vector<VkImageView>& attachments,
    const VkRenderPass& renderPass,
    const VkExtent2D& swapChainExtent,
    VkFramebuffer& framebuffer);
}
