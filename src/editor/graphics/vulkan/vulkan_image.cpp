#include "vulkan_image.hpp"

#include "vulkan_utils.hpp"

#include <core/logger.hpp>

#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace Zeus
{
bool createVkImageView(
    const VkDevice& device,
    const VkImage& image,
    const VkFormat format,
    const VkImageAspectFlags aspectFlags,
    uint32_t mipLevels,
    VkImageView& imageView)
{
    VkImageViewCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .components =
            {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
        .subresourceRange =
            {
                .aspectMask = aspectFlags,
                .baseMipLevel = 0,
                .levelCount = mipLevels,
                // Usage: Stereograhic 3D application
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
    };

    VkResult result{
        vkCreateImageView(device, &createInfo, nullptr, &imageView)};
    if (result != VK_SUCCESS)
    {
        error("Failed to create image view. {}", vkResultToString(result));
    }

    return result == VK_SUCCESS;
}
}
