#include "vulkan_image.hpp"

#include "vulkan_debug.hpp"

#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cstdint>
#include <vector>

namespace Zeus
{
VkFormat findSupportedImageFormat(
    VkPhysicalDevice physicalDevice,
    const std::vector<VkFormat>& candidates,
    VkImageTiling tiling,
    VkFormatFeatureFlags formatFeatureFlags)
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(
            physicalDevice,
            format,
            &properties);

        if (tiling == VK_IMAGE_TILING_LINEAR &&
            (properties.linearTilingFeatures & formatFeatureFlags) ==
                formatFeatureFlags)
        {
            return format;
        }
        else if (
            tiling == VK_IMAGE_TILING_OPTIMAL &&
            (properties.optimalTilingFeatures & formatFeatureFlags) ==
                formatFeatureFlags)
        {
            return format;
        }
    }

    VKCHECK(VK_ERROR_FORMAT_NOT_SUPPORTED, "Failed to find supported format.");
    return VK_FORMAT_UNDEFINED;
}

std::uint32_t calcMipLevels(std::uint32_t width, std::uint32_t height)
{
    // log2 how many times that dimension can be divided by 2
    // floor for cases where the largest dimension is not a power of 2
    // 1 is added so that the original image has a mip level
    auto mipLevels = std::floor(std::log2(std::max(width, height))) + 1;
    return static_cast<std::uint32_t>(mipLevels);
}
}
