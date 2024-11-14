#pragma once

#include "math/definitions.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
void cmdClearColorImage(
    VkCommandBuffer commandBuffer,
    VkImage image,
    const Vector4f& color,
    VkImageLayout imageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

void cmdClearDepthStencilImage(
    VkCommandBuffer commandBuffer,
    VkImage image,
    const VkClearDepthStencilValue& clearValue,
    VkImageLayout imageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

VkFormat findSupportedImageFormat(
    VkPhysicalDevice physicalDevice,
    const std::vector<VkFormat>& candidates,
    VkImageTiling tiling,
    VkFormatFeatureFlags features);

void transitionImageLayout(
    VkCommandBuffer commandBuffer,
    VkImage image,
    VkFormat format,
    VkImageLayout currentLayout,
    VkImageLayout newLayout);

void blitImage(
    VkCommandBuffer commandBuffer,
    VkImage srcImage,
    VkImage dstImage,
    VkExtent2D srcSize,
    VkExtent2D dstSize);

std::uint32_t calcMipLevels(std::uint32_t width, std::uint32_t height);
}
