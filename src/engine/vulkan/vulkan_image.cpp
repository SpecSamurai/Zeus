#include "vulkan_image.hpp"
#include "vulkan/MemoryAllocator.hpp"

#include <core/logger.hpp>
#include <vulkan/vulkan_utils.hpp>

#include <vulkan/vulkan_core.h>

#include <cstdint>

namespace Zeus
{
bool createVkImage(
    const VkDevice& device,
    const VkPhysicalDevice& physicalDevice,
    const ImageConfig& config,
    VkImage& image,
    VkDeviceMemory& imageMemory)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = config.width;
    imageInfo.extent.height = config.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = config.mipLevels;
    imageInfo.arrayLayers = 1;
    imageInfo.format = config.format;
    imageInfo.tiling = config.tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = config.usage;
    imageInfo.samples = config.numSamples;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult createImageResult{
        vkCreateImage(device, &imageInfo, nullptr, &image)
    };

    VKCHECK(createImageResult, "Failed to create image. {}");

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(device, image, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = findMemoryType(
        physicalDevice,
        memoryRequirements.memoryTypeBits,
        config.properties);

    VkResult allocateResult{
        vkAllocateMemory(device, &allocateInfo, nullptr, &imageMemory)
    };

    VKCHECK(allocateResult, "Failed to allocate image memory.");

    vkBindImageMemory(device, image, imageMemory, 0);

    return true;
}

VkResult createVkImageView(
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
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = aspectFlags;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = mipLevels;
    // Usage: Stereograhic 3D application
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    VkResult result{
        vkCreateImageView(device, &createInfo, nullptr, &imageView)
    };

    VKCHECK(result, "Failed to create image view.");

    return result;
}
}
