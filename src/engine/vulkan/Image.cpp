#include "Image.hpp"

#include "VkContext.hpp"
#include "rhi/vulkan_debug.hpp"

#include <vulkan/vulkan_core.h>

#include <string>

namespace Zeus
{
Image::Image(
    const ImageType type,
    VkExtent3D extent,
    const VkFormat format,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags memoryPropertyFlags,
    const char* name,
    const std::uint32_t mipLevels,
    VkImageLayout layout,
    VkImageTiling tiling)
    : m_name{ name },
      m_type{ type },
      m_extent{ extent },
      m_format{ format },
      m_layout{ layout },
      m_mipLevels{ mipLevels },
      m_usage{ usage }
{
    CreateImage(memoryPropertyFlags, tiling);
    CreateImageView();
}

Image::~Image()
{
    if (m_handle == VK_NULL_HANDLE)
        return;

    VkContext::GetDeletionQueue().Add(ResourceType::Image, this);
}

void Image::Destroy()
{
    vkDestroyImageView(
        VkContext::GetLogicalDevice(),
        m_view,
        allocationCallbacks.get());

    vmaDestroyImage(VkContext::GetAllocator(), m_handle, m_allocation);

    m_handle = VK_NULL_HANDLE;
    m_view = VK_NULL_HANDLE;
    m_allocation = VK_NULL_HANDLE;
}

void Image::SaveToFile(const std::filesystem::path& filePath)
{
}

void Image::LoadFromFile(const std::filesystem::path& filePath)
{
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

void Image::CreateImage(
    VkMemoryPropertyFlags memoryPropertyFlags,
    VkImageTiling tiling)
{
    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext = nullptr;
    imageCreateInfo.flags = 0;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = m_format;
    imageCreateInfo.extent = m_extent;
    imageCreateInfo.mipLevels = m_mipLevels;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = tiling;
    imageCreateInfo.usage = m_usage;
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
        assert(false && "Unsupported Memory property flag.");
    }

    VKCHECK(
        vmaCreateImage(
            VkContext::GetAllocator(),
            &imageCreateInfo,
            &allocationCreateInfo,
            &m_handle,
            &m_allocation,
            &m_info),
        "Failed to create image.");

    VkContext::SetDebugName(VK_OBJECT_TYPE_IMAGE, m_handle, m_name);
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

    switch (m_type)
    {
    case ImageType::Texture2D:
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        break;
    case ImageType::Texture3D:
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_3D;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        break;
        break;
    case ImageType::Cube:
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 6;
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
            VkContext::GetLogicalDevice(),
            &createInfo,
            allocationCallbacks.get(),
            &m_view),
        "Failed to create image view.");

    if (m_name != nullptr)
    {
        std::string name(m_name);
        name += "_View";

        VkContext::SetDebugName(
            VK_OBJECT_TYPE_IMAGE_VIEW,
            m_view,
            name.c_str());
    }
}
}
