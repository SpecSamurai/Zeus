#include "vulkan_image.hpp"

#include "vulkan_utils.hpp"

#include <core/logger.hpp>

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
    VkImageView& imageView)
{
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = format;

    VkComponentMapping componentMapping{};
    componentMapping.r = VK_COMPONENT_SWIZZLE_IDENTITY,
    componentMapping.g = VK_COMPONENT_SWIZZLE_IDENTITY,
    componentMapping.b = VK_COMPONENT_SWIZZLE_IDENTITY,
    componentMapping.a = VK_COMPONENT_SWIZZLE_IDENTITY,
    createInfo.components = componentMapping;

    VkImageSubresourceRange imageSubresourceRange{};
    imageSubresourceRange.aspectMask = aspectFlags;
    imageSubresourceRange.baseMipLevel = 0;
    imageSubresourceRange.levelCount = mipLevels;
    // Usage: Stereograhic 3D application
    imageSubresourceRange.baseArrayLayer = 0;
    imageSubresourceRange.layerCount = 1;
    createInfo.subresourceRange = imageSubresourceRange;

    VkResult result{
        vkCreateImageView(device, &createInfo, nullptr, &imageView)
    };

    if (result != VK_SUCCESS)
    {
        error("Failed to create image view. {}", vkResultToString(result));
    }

    return result == VK_SUCCESS;
}
}
