#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>

namespace Zeus
{
struct ImageConfig
{
    std::uint32_t width;
    std::uint32_t height;
    std::uint32_t mipLevels;
    VkSampleCountFlagBits numSamples;
    VkFormat format;
    VkImageTiling tiling;
    VkImageUsageFlags usage;
    VkMemoryPropertyFlags properties;
};

bool createVkImage(
    const VkDevice& device,
    const VkPhysicalDevice& physicalDevice,
    const ImageConfig& config,
    VkImage& image,
    VkDeviceMemory& imageMemory);

bool createVkImageView(
    const VkDevice& device,
    const VkImage& image,
    const VkFormat format,
    const VkImageAspectFlags aspectFlags,
    const std::uint32_t mipLevels,
    VkImageView& imageView);
}
