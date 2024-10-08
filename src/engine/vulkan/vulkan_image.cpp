#include "vulkan_image.hpp"

#include "math/definitions.hpp"
#include "vulkan_debug.hpp"
#include "vulkan_memory.hpp"

#include <vulkan/utility/vk_format_utils.h>
#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cstdint>
#include <vector>

namespace Zeus
{
void destroyImage(VkDevice device, VmaAllocator allocator, Image& image)
{
    vkDestroyImageView(device, image.imageView, allocationCallbacks.get());
    vmaDestroyImage(allocator, image.image, image.allocation);
}

VkResult create2DImage(
    VmaAllocator allocator,
    Image& image,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags memoryPropertyFlags,
    std::uint32_t mipLevels,
    VkImageTiling tiling)
{
    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext = nullptr;
    imageCreateInfo.flags = 0;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = image.imageFormat;
    imageCreateInfo.extent = image.imageExtent;
    imageCreateInfo.mipLevels = mipLevels;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = tiling;
    imageCreateInfo.usage = usage;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.queueFamilyIndexCount = 0;
    imageCreateInfo.pQueueFamilyIndices = nullptr;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VmaAllocationCreateInfo allocationCreateInfo{};
    allocationCreateInfo.requiredFlags = memoryPropertyFlags;

    switch (memoryPropertyFlags)
    {
    case VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT:
        allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        break;

    default:
        assert(0);
    }

    VkResult result{ vmaCreateImage(
        allocator,
        &imageCreateInfo,
        &allocationCreateInfo,
        &image.image,
        &image.allocation,
        &image.allocationInfo) };

    VKCHECK(result, "Failed to create image.");

    return result;
}

VkResult create2DImageView(
    VkDevice device,
    Image& image,
    const VkImageAspectFlags aspectFlags,
    std::uint32_t mipLevels)
{
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.image = image.image;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = image.imageFormat;
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

    VkResult result{ vkCreateImageView(
        device,
        &createInfo,
        allocationCallbacks.get(),
        &image.imageView) };

    VKCHECK(result, "Failed to create image view.");

    return result;
}

void cmdClearColorImage(
    VkCommandBuffer commandBuffer,
    VkImage image,
    const Vector4f& color,
    VkImageLayout imageLayout)
{
    VkClearColorValue clearValue{ { color.r, color.g, color.b, color.a } };

    VkImageSubresourceRange subresourceRange{};
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

    vkCmdClearColorImage(
        commandBuffer,
        image,
        imageLayout,
        &clearValue,
        1,
        &subresourceRange);
}

void cmdClearDepthStencilImage(
    VkCommandBuffer commandBuffer,
    VkImage image,
    const VkClearDepthStencilValue& clearValue,
    VkImageLayout imageLayout)
{
    VkImageSubresourceRange subresourceRange{};
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

    vkCmdClearDepthStencilImage(
        commandBuffer,
        image,
        imageLayout,
        &clearValue,
        1,
        &subresourceRange);
}

VkResult createVkSampler(
    VkDevice device,
    VkSampler& sampler,
    VkFilter magFilter,
    VkFilter minFilter,
    VkSamplerMipmapMode mipmapMode,
    VkSamplerCreateFlags flags)
{
    VkSamplerCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.flags = flags;
    createInfo.magFilter = magFilter;
    createInfo.minFilter = minFilter;
    createInfo.mipmapMode = mipmapMode;
    createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.mipLodBias = 0.f;
    createInfo.anisotropyEnable = VK_FALSE; // VK_TRUE;
    createInfo.maxAnisotropy = 0.f; // properties.limits.maxSamplerAnisotropy
    createInfo.compareEnable = VK_FALSE;
    createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    createInfo.minLod = 0.f;
    createInfo.maxLod = VK_LOD_CLAMP_NONE; // mipLevels
    createInfo.borderColor =
        VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK; // VK_BORDER_COLOR_INT_OPAQUE_BLACK
    createInfo.unnormalizedCoordinates = VK_FALSE;

    VkResult result{ vkCreateSampler(
        device,
        &createInfo,
        allocationCallbacks.get(),
        &sampler) };

    VKCHECK(result, "Failed to create sampler.");

    return result;
}

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

void transitionImageLayout(
    VkCommandBuffer commandBuffer,
    VkImage& image,
    VkFormat format,
    VkImageLayout currentLayout,
    VkImageLayout newLayout)
{
    VkImageMemoryBarrier2 imageMemoryBarrier{};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    imageMemoryBarrier.pNext = nullptr;

    // if (currentLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
    //     newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    // {
    //     imageMemoryBarrier.srcStageMask =
    //         VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT; //
    //         VK_PIPELINE_STAGE_2_NONE;
    //     imageMemoryBarrier.srcAccessMask = VK_ACCESS_2_NONE;
    //     imageMemoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    //     imageMemoryBarrier.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    // }
    // else if (
    //     currentLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
    //     (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL ||
    //      newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL))
    // {
    //     imageMemoryBarrier.srcStageMask =
    //         VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT; //
    //         VK_PIPELINE_STAGE_2_NONE;
    //     imageMemoryBarrier.srcAccessMask = VK_ACCESS_2_NONE;
    //     imageMemoryBarrier.dstStageMask =
    //         VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT;
    //     imageMemoryBarrier.dstAccessMask =
    //         VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
    //         VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    // }
    // else if (
    //     currentLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
    //     newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    // {
    //     imageMemoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    //     imageMemoryBarrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    //     imageMemoryBarrier.dstStageMask =
    //         VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
    //     imageMemoryBarrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
    // }
    // else
    {
        // error("Unsupported image layout transition.");
        imageMemoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
        imageMemoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        imageMemoryBarrier.dstAccessMask =
            VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
        // assert(false);
    }

    imageMemoryBarrier.oldLayout = currentLayout;
    imageMemoryBarrier.newLayout = newLayout;

    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    imageMemoryBarrier.image = image;

    VkImageAspectFlags aspectMask;
    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL ||
        newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        if (vkuFormatIsDepthAndStencil(format))
        {
            aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    }
    else
    {
        aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    imageMemoryBarrier.subresourceRange.aspectMask = aspectMask;
    imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
    imageMemoryBarrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

    VkDependencyInfo dependencyInfo{};
    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependencyInfo.pNext = nullptr;

    dependencyInfo.imageMemoryBarrierCount = 1;
    dependencyInfo.pImageMemoryBarriers = &imageMemoryBarrier;

    vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
}

void blitImage(
    VkCommandBuffer commandBuffer,
    VkImage srcImage,
    VkImage dstImage,
    VkExtent2D srcSize,
    VkExtent2D dstSize)
{
    VkImageBlit2 blitRegion{};
    blitRegion.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;

    blitRegion.srcOffsets[1].x = static_cast<std::int32_t>(srcSize.width);
    blitRegion.srcOffsets[1].y = static_cast<std::int32_t>(srcSize.height);
    blitRegion.srcOffsets[1].z = 1;

    blitRegion.dstOffsets[1].x = static_cast<std::int32_t>(dstSize.width);
    blitRegion.dstOffsets[1].y = static_cast<std::int32_t>(dstSize.height);
    blitRegion.dstOffsets[1].z = 1;

    blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blitRegion.srcSubresource.baseArrayLayer = 0;
    blitRegion.srcSubresource.layerCount = 1;
    blitRegion.srcSubresource.mipLevel = 0;

    blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blitRegion.dstSubresource.baseArrayLayer = 0;
    blitRegion.dstSubresource.layerCount = 1;
    blitRegion.dstSubresource.mipLevel = 0;

    VkBlitImageInfo2 blitImageInfo{};
    blitImageInfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
    blitImageInfo.srcImage = srcImage;
    blitImageInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    blitImageInfo.dstImage = dstImage;
    blitImageInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

    blitImageInfo.regionCount = 1;
    blitImageInfo.pRegions = &blitRegion;
    blitImageInfo.filter = VK_FILTER_LINEAR;

    vkCmdBlitImage2(commandBuffer, &blitImageInfo);
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
