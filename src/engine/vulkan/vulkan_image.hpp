#pragma once

#include "vulkan_types.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
void destroyImage(VkDevice device, VmaAllocator allocator, Image& image);

VkResult create2DImage(
    VmaAllocator allocator,
    Image& image,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags memoryPropertyFlags,
    std::uint32_t mipLevels = 1,
    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL);

VkResult create2DImageView(
    VkDevice device,
    Image& image,
    const VkImageAspectFlags aspectFlags,
    std::uint32_t mipLevels = 1);

VkFormat findSupportedImageFormat(
    VkPhysicalDevice physicalDevice,
    const std::vector<VkFormat>& candidates,
    VkImageTiling tiling,
    VkFormatFeatureFlags features);

void transitionImageLayout(
    VkCommandBuffer commandBuffer,
    VkImage& image,
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
