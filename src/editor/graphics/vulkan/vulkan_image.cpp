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
    // Format of  an image view; it must be compatible with the image’s format
    // but may not be the same format (that is, it may be a different format but
    // with the same number of bits per pixel).
    // components – Mapping of an image components into a vector returned in the
    // shader by texturing operations. This applies only to read operations
    // (sampling), but since we are using an image as a color attachment (we are
    // rendering into an image) we must set the so-called identity mapping (R
    // component into R, G -> G, and so on) or just use “identity” value
    // (VK_COMPONENT_SWIZZLE_IDENTITY).
    // subresourceRange – Describes the set of mipmap levels and array layers
    // that will be accessible to a view. If our image is mipmapped, we may
    // specify the specific mipmap level we want to render to (and in case of
    // render targets we must specify exactly one mipmap level of one array
    // layer).
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
        vkCreateImageView(device, &createInfo, nullptr, &imageView)
    };

    if (result != VK_SUCCESS)
    {
        error("Failed to create image view. {}", vkResultToString(result));
    }

    return result == VK_SUCCESS;
}
}
