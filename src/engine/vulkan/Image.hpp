#pragma once

#include "Definitions.hpp"
#include "rhi/vulkan_memory.hpp"

#include <vulkan/utility/vk_format_utils.h>
#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cstdint>
#include <filesystem>

namespace Zeus
{
class Image
{
public:
    Image() = default;

    Image(
        const ImageType type,
        VkExtent3D extent,
        const VkFormat format,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags memoryPropertyFlags,
        const char* name = nullptr,
        const std::uint32_t mipLevels = 1,
        VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED,
        VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL);

    ~Image();
    void Destroy();

    void SaveToFile(const std::filesystem::path& filePath);
    void LoadFromFile(const std::filesystem::path& filePath);

    bool IsDepthFormat() const;
    bool IsStencilFormat() const;
    bool IsDepthAndStencil() const;
    bool IsColorFormat() const;

    VkImage GetHandle() const;
    VkImageView GetView() const;
    VmaAllocation GetAllocation() const;
    const VmaAllocationInfo& GetInfo() const;

    ImageType GetType() const;
    const VkExtent3D& GetExtent() const;
    std::uint32_t GetWidth() const;
    std::uint32_t GetHeight() const;
    std::uint32_t GetDepth() const;
    VkFormat GetFormat() const;
    VkImageLayout GetLayout() const;
    std::uint32_t GetMipLevels() const;

private:
    void CreateImage(
        VkMemoryPropertyFlags memoryPropertyFlags,
        VkImageTiling tiling);

    void CreateImageView();

private:
    VkImage m_handle{ VK_NULL_HANDLE };
    VkImageView m_view{ VK_NULL_HANDLE };
    VmaAllocation m_allocation{ VK_NULL_HANDLE };
    VmaAllocationInfo m_info{};

    const char* m_name{ nullptr };

    ImageType m_type;
    VkExtent3D m_extent;
    VkFormat m_format;
    VkImageLayout m_layout;
    std::uint32_t m_mipLevels;
    VkImageTiling m_tiling;
    VkImageUsageFlags m_usage;
    VkMemoryPropertyFlags m_memoryPropertyFlags;
};
}
