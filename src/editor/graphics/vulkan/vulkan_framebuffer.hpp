#pragma once

#include <vulkan/vulkan_core.h>

#include <array>

namespace Zeus
{
bool createVkFramebuffer(
    const VkDevice& device,
    const std::array<VkImageView, 3>& attachments,
    const VkRenderPass& renderPass,
    const VkExtent2D& swapChainExtent,
    VkFramebuffer& framebuffer);
}
