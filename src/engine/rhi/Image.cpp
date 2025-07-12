#include "Image.hpp"

#include "Buffer.hpp"
#include "VkContext.hpp"
#include "rhi/CommandBuffer.hpp"
#include "rhi/vulkan/vulkan_image.hpp"
#include "vulkan/vulkan_debug.hpp"

#include <stb_image.h>
#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cstdint>
#include <format>
#include <span>
#include <string>
#include <string_view>

namespace Zeus
{
Image::Image(
    std::string_view name,
    const ImageType type,
    VkExtent3D extent,
    const VkFormat format,
    VkImageUsageFlags usage,
    const std::uint32_t mipLevels,
    const std::uint32_t arrayLayers,
    VkMemoryPropertyFlags memoryPropertyFlags,
    VkImageTiling tiling)
    : m_name{ name },
      m_type{ type },
      m_extent{ extent },
      m_format{ format },
      m_layout{ VK_IMAGE_LAYOUT_UNDEFINED },
      m_mipLevels{ mipLevels },
      m_arrayLayers{ type == ImageType::Cube ? 6 : arrayLayers },
      m_usage{ usage }
{
    CreateImage(memoryPropertyFlags, tiling);
    CreateImageView();

    VkContext::SetDebugName(VK_OBJECT_TYPE_IMAGE, m_handle, m_name);
    VkContext::SetDebugName(
        VK_OBJECT_TYPE_IMAGE_VIEW,
        m_view,
        m_name.empty() ? "" : std::format("{}_View", m_name));
}

Image::Image(Image&& other) noexcept
    : m_handle{ other.m_handle },
      m_view{ other.m_view },
      m_allocation{ other.m_allocation },
      m_info{ other.m_info },
      m_name{ other.m_name },
      m_type{ other.m_type },
      m_extent{ other.m_extent },
      m_format{ other.m_format },
      m_layout{ other.m_layout },
      m_mipLevels{ other.m_mipLevels },
      m_arrayLayers{ other.m_arrayLayers },
      m_tiling{ other.m_tiling },
      m_usage{ other.m_usage },
      m_memoryPropertyFlags{ other.m_memoryPropertyFlags },
      m_aspectMask{ other.m_aspectMask }
{
    other.m_handle = VK_NULL_HANDLE;
    other.m_view = VK_NULL_HANDLE;
    other.m_allocation = VK_NULL_HANDLE;
    other.m_info = {};
}

Image& Image::operator=(Image&& other)
{
    if (this != &other)
    {
        if (m_handle != VK_NULL_HANDLE)
        {
            Destroy();
        }

        m_handle = other.m_handle;
        m_view = other.m_view;
        m_allocation = other.m_allocation;
        m_info = other.m_info;
        m_name = other.m_name;
        m_type = other.m_type;
        m_extent = other.m_extent;
        m_format = other.m_format;
        m_layout = other.m_layout;
        m_mipLevels = other.m_mipLevels;
        m_arrayLayers = other.m_arrayLayers;
        m_tiling = other.m_tiling;
        m_usage = other.m_usage;
        m_memoryPropertyFlags = other.m_memoryPropertyFlags;
        m_aspectMask = other.m_aspectMask;

        other.m_handle = VK_NULL_HANDLE;
        other.m_view = VK_NULL_HANDLE;
        other.m_allocation = VK_NULL_HANDLE;
        other.m_info = {};
    }

    return *this;
}

Image::~Image()
{
    if (m_handle == VK_NULL_HANDLE)
        return;

    VkContext::DeletionQueue().AddImage(m_handle, m_view, m_allocation);
}

void Image::Destroy()
{
    vkDestroyImageView(
        VkContext::LogicalDevice(),
        m_view,
        allocationCallbacks.get());

    vmaDestroyImage(VkContext::Allocator(), m_handle, m_allocation);

    m_handle = VK_NULL_HANDLE;
    m_view = VK_NULL_HANDLE;
    m_allocation = VK_NULL_HANDLE;
}

void Image::CopyBufferToImage(const Buffer& srcBuffer)
{
    VkContext::Device().CmdImmediateSubmit(
        [&](const CommandBuffer& commandBuffer) {
            SetLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

            std::vector<VkBufferImageCopy2> regions(m_arrayLayers);

            std::size_t offset{ 0 };
            std::size_t layerSize{ m_extent.width * m_extent.height * 4 };
            for (std::uint32_t layer{ 0 }; layer < m_arrayLayers; ++layer)
            {
                regions[layer].sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2;
                regions[layer].pNext = nullptr;
                regions[layer].bufferOffset = offset;
                regions[layer].bufferRowLength = 0;
                regions[layer].bufferImageHeight = 0;
                regions[layer].imageSubresource.aspectMask = m_aspectMask;
                regions[layer].imageSubresource.mipLevel = 0;
                regions[layer].imageSubresource.layerCount = 1;
                regions[layer].imageSubresource.baseArrayLayer = layer;
                regions[layer].imageOffset = {};
                regions[layer].imageExtent = m_extent;

                offset += layerSize;
            }

            commandBuffer.CopyBufferToImage(
                srcBuffer.GetHandle(),
                m_handle,
                regions);

            if (m_mipLevels > 1)
            {
                GenerateMipmaps(commandBuffer.GetHandle());
            }

            SetLayout(commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        });
}

void Image::BlitToImage(
    const CommandBuffer& commandBuffer,
    const Image& dstImage)
{
    assert(GetType() == dstImage.GetType());
    assert(GetLayout() == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    assert(dstImage.GetLayout() == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    assert(
        GetWidth() <= dstImage.GetWidth() &&
        GetHeight() <= dstImage.GetHeight());

    VkImageBlit2 region{};
    region.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;

    region.srcSubresource.aspectMask = GetAspectMask();
    region.srcSubresource.mipLevel = GetMipLevels();
    region.srcSubresource.baseArrayLayer = 0;
    region.srcSubresource.layerCount = GetArrayLayers();

    region.srcOffsets[1].x = static_cast<std::int32_t>(GetWidth());
    region.srcOffsets[1].y = static_cast<std::int32_t>(GetHeight());
    region.srcOffsets[1].z = static_cast<std::int32_t>(GetDepth());

    region.dstSubresource.aspectMask = dstImage.GetAspectMask();
    region.dstSubresource.mipLevel = dstImage.GetMipLevels();
    region.dstSubresource.baseArrayLayer = 0;
    region.dstSubresource.layerCount = dstImage.m_arrayLayers;

    region.dstOffsets[1].x = static_cast<std::int32_t>(dstImage.GetWidth());
    region.dstOffsets[1].y = static_cast<std::int32_t>(dstImage.GetHeight());
    region.dstOffsets[1].z = static_cast<std::int32_t>(dstImage.GetDepth());

    bool isSameSize{ GetWidth() == dstImage.GetWidth() &&
                     GetHeight() == dstImage.GetHeight() };

    VkImageBlit2 regions[1]{ region };
    commandBuffer.BlitImage(
        m_handle,
        dstImage.GetHandle(),
        regions,
        isSameSize ? VK_FILTER_NEAREST : VK_FILTER_LINEAR);
}

void Image::Update(void* data, std::size_t dataSize)
{
    assert(m_handle != VK_NULL_HANDLE);
    assert(data != nullptr && "Invalid data");
    assert(dataSize > 0 && dataSize <= m_info.size && "Invalid size");

    Buffer staging(
        "Buffer_image_update",
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        dataSize,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        true);

    staging.Update(data, dataSize);

    VkContext::Device().CmdImmediateSubmit(
        [&](const CommandBuffer& commandBuffer) {
            SetLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

            std::uint32_t layerCount{ m_type == ImageType::Cube ? 6u : 1u };
            std::vector<VkBufferImageCopy2> regions(layerCount);

            std::size_t offset{ 0 };
            std::size_t layerSize{ m_extent.width * m_extent.height * 4 };
            for (std::uint32_t layer{ 0 }; layer < layerCount; ++layer)
            {
                regions[layer].sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2;
                regions[layer].pNext = nullptr;
                regions[layer].bufferOffset = offset;
                regions[layer].bufferRowLength = 0;
                regions[layer].bufferImageHeight = 0;
                regions[layer].imageSubresource.aspectMask = m_aspectMask;
                regions[layer].imageSubresource.mipLevel = 0;
                regions[layer].imageSubresource.layerCount = 1;
                regions[layer].imageSubresource.baseArrayLayer = layer;
                regions[layer].imageOffset = {};
                regions[layer].imageExtent = m_extent;

                offset += layerSize;
            }

            commandBuffer.CopyBufferToImage(
                staging.GetHandle(),
                m_handle,
                regions);

            if (m_mipLevels > 1)
            {
                GenerateMipmaps(commandBuffer.GetHandle());
            }

            SetLayout(commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        });

    staging.Destroy();
}

void Image::SaveToFile(const std::filesystem::path& filePath)
{
}

Image Image::LoadFromFiles(
    std::string_view name,
    ImageType type,
    std::span<std::string_view> files,
    bool mipmapped)
{
    int width, height, channels;
    stbi_uc* firstLayerData{
        stbi_load(files[0].data(), &width, &height, &channels, STBI_rgb_alpha)
    };

    VkDeviceSize layerSize{
        static_cast<std::uint32_t>(width * height * 4),
    };

    Buffer staging(
        "Buffer_Staging_Image_LoadFromFiles",
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        layerSize,
        files.size(),
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        true);

    staging.UpdateAt(firstLayerData, layerSize, 0);

    stbi_image_free(firstLayerData);

    for (std::uint32_t layer{ 1 }; layer < files.size(); ++layer)
    {
        stbi_uc* data{ stbi_load(
            files[layer].data(),
            &width,
            &height,
            &channels,
            STBI_rgb_alpha) };

        staging.UpdateAt(data, layerSize, layer);

        stbi_image_free(data);
    }

    VkExtent3D imageExtent{
        .width = static_cast<std::uint32_t>(width),
        .height = static_cast<std::uint32_t>(height),
        .depth = 1,
    };
    Image image(
        name,
        type,
        imageExtent,
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        mipmapped ? calcMipLevels(imageExtent.width, imageExtent.height) : 1);

    image.CopyBufferToImage(staging);

    return image;
}

Image Image::LoadFromFile(
    std::string_view name,
    const std::string& filePath,
    bool mipmapped)
{
    std::string_view files[1]{ filePath.c_str() };
    return LoadFromFiles(name, ImageType::Texture2D, files, mipmapped);
}

void Image::SetLayout(const CommandBuffer& commandBuffer, VkImageLayout layout)
{
    if (m_layout == layout)
        return;

    commandBuffer.TransitionImageLayout(m_handle, m_format, m_layout, layout);

    m_layout = layout;
}

bool Image::IsDepthFormat() const
{
    return vkuFormatIsDepthOrStencil(m_format) &&
           !vkuFormatIsStencilOnly(m_format);
}

bool Image::IsStencilFormat() const
{
    return vkuFormatIsDepthOrStencil(m_format) &&
           !vkuFormatIsDepthOnly(m_format);
}

bool Image::IsDepthAndStencil() const
{
    return vkuFormatIsDepthAndStencil(m_format);
}

bool Image::IsColorFormat() const
{
    return !vkuFormatIsDepthOrStencil(m_format);
}

VkImage Image::GetHandle() const
{
    return m_handle;
}

VkImageView Image::GetView() const
{
    return m_view;
}

VmaAllocation Image::GetAllocation() const
{
    return m_allocation;
}

const VmaAllocationInfo& Image::GetInfo() const
{
    return m_info;
}

ImageType Image::GetType() const
{
    return m_type;
}

const VkExtent3D& Image::GetExtent() const
{
    return m_extent;
}

std::uint32_t Image::GetWidth() const
{
    return m_extent.width;
}

std::uint32_t Image::GetHeight() const
{
    return m_extent.height;
}

std::uint32_t Image::GetDepth() const
{
    return m_extent.depth;
}

VkFormat Image::GetFormat() const
{
    return m_format;
}

VkImageLayout Image::GetLayout() const
{
    return m_layout;
}

std::uint32_t Image::GetMipLevels() const
{
    return m_mipLevels;
}

std::uint32_t Image::GetArrayLayers() const
{
    return m_arrayLayers;
}

VkImageAspectFlags Image::GetAspectMask() const
{
    return m_aspectMask;
}

void Image::CreateImage(
    VkMemoryPropertyFlags memoryPropertyFlags,
    VkImageTiling tiling)
{
    // if mipLevels > 1 then must be VK_IMAGE_USAGE_TRANSFER_SRC_BIT
    assert(!(m_mipLevels > 1) || (m_usage & VK_IMAGE_USAGE_TRANSFER_SRC_BIT));

    if (m_mipLevels > 1)
    {
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(
            VkContext::Device().GetPhysicalDevice(),
            m_format,
            &formatProperties);

        assert(
            (formatProperties.optimalTilingFeatures &
             VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT) &&
            "Format does not support linear blitting.");
    }

    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext = nullptr;
    imageCreateInfo.flags =
        m_type == ImageType::Cube ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
    imageCreateInfo.imageType =
        m_type == ImageType::Texture3D ? VK_IMAGE_TYPE_3D : VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = m_format;
    imageCreateInfo.extent = m_extent;
    imageCreateInfo.mipLevels = m_mipLevels;
    imageCreateInfo.arrayLayers = m_arrayLayers;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = tiling;
    imageCreateInfo.usage = m_usage;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.queueFamilyIndexCount = 0;
    imageCreateInfo.pQueueFamilyIndices = nullptr;
    // spec: initialLayout must be VK_IMAGE_LAYOUT_UNDEFINED
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VmaAllocationCreateInfo allocationCreateInfo{};
    allocationCreateInfo.requiredFlags = memoryPropertyFlags;

    switch (memoryPropertyFlags)
    {
    case VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT:
        allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        break;
    default:
        assert(false && "Unsupported Memory property flag.");
    }

    VKCHECK(
        vmaCreateImage(
            VkContext::Allocator(),
            &imageCreateInfo,
            &allocationCreateInfo,
            &m_handle,
            &m_allocation,
            &m_info),
        "Failed to create image.");
}

void Image::CreateImageView()
{
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.image = m_handle;
    createInfo.format = m_format;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = m_mipLevels;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = m_arrayLayers;

    switch (m_type)
    {
    case ImageType::Texture2D:
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        break;
    case ImageType::Texture3D:
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_3D;
        break;
    case ImageType::Texture2DArray:
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
        break;
    case ImageType::Cube:
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
        break;
    default:
        assert(false && "Unsupported type");
        break;
    }

    VkImageAspectFlags aspectMask{};
    if (IsColorFormat())
    {
        aspectMask |= VK_IMAGE_ASPECT_COLOR_BIT;
    }
    else
    {
        if (IsDepthFormat())
        {
            aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
        }

        if (IsStencilFormat())
        {
            aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    }

    createInfo.subresourceRange.aspectMask = aspectMask;

    VKCHECK(
        vkCreateImageView(
            VkContext::LogicalDevice(),
            &createInfo,
            allocationCallbacks.get(),
            &m_view),
        "Failed to create image view.");

    m_aspectMask = aspectMask;
}

void Image::GenerateMipmaps(VkCommandBuffer commandBuffer)
{
    assert(m_layout & VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    std::int32_t mipmapWidth{ static_cast<int32_t>(GetWidth()) };
    std::int32_t mipmapHeight{ static_cast<int32_t>(GetHeight()) };

    for (std::uint32_t mip{ 0 }; mip < m_mipLevels; ++mip)
    {
        VkImageMemoryBarrier2 imageBarrier{};
        imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;

        imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
        imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        imageBarrier.dstAccessMask =
            VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

        imageBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        imageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

        imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageBarrier.subresourceRange.baseArrayLayer = 0;
        imageBarrier.subresourceRange.layerCount = 1;

        imageBarrier.subresourceRange.baseMipLevel = mip;
        imageBarrier.subresourceRange.levelCount = 1;
        imageBarrier.image = m_handle;

        VkDependencyInfo depInfo{};
        depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;

        depInfo.imageMemoryBarrierCount = 1;
        depInfo.pImageMemoryBarriers = &imageBarrier;

        vkCmdPipelineBarrier2(commandBuffer, &depInfo);

        if (mip < m_mipLevels - 1)
        {
            VkImageBlit2 blitRegion{};
            blitRegion.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;

            blitRegion.srcOffsets[1] = {
                .x = mipmapWidth,
                .y = mipmapHeight,
                .z = 1,
            };

            blitRegion.dstOffsets[1] = {
                .x = mipmapWidth > 1 ? mipmapWidth / 2 : 1,
                .y = mipmapHeight > 1 ? mipmapHeight / 2 : 1,
                .z = 1,
            };

            blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blitRegion.srcSubresource.baseArrayLayer = 0;
            blitRegion.srcSubresource.layerCount = 1;
            blitRegion.srcSubresource.mipLevel = mip;

            blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blitRegion.dstSubresource.baseArrayLayer = 0;
            blitRegion.dstSubresource.layerCount = 1;
            blitRegion.dstSubresource.mipLevel = mip + 1;

            VkBlitImageInfo2 blitInfo{};
            blitInfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;

            blitInfo.dstImage = m_handle;
            blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            blitInfo.srcImage = m_handle;
            blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            blitInfo.filter = VK_FILTER_LINEAR;
            blitInfo.regionCount = 1;
            blitInfo.pRegions = &blitRegion;

            vkCmdBlitImage2(commandBuffer, &blitInfo);

            if (mipmapWidth > 1)
                mipmapWidth /= 2;
            if (mipmapHeight > 1)
                mipmapHeight /= 2;
        }
    }

    m_layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
}
}
