#pragma once

#include "rhi/Buffer.hpp"
#include "rhi/CommandBuffer.hpp"
#include "vulkan/vulkan_memory.hpp"

#include <vulkan/utility/vk_format_utils.h>
#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cstdint>
#include <filesystem>
#include <string_view>

namespace Zeus
{
enum class ImageType : std::uint8_t
{
    Cube,
    Texture2D,
    Texture2DArray,
    Texture3D,
};

class Image
{
public:
    Image() = default;

    Image(
        std::string_view name,
        const ImageType type,
        VkExtent3D extent,
        const VkFormat format,
        VkImageUsageFlags usage,
        const std::uint32_t mipLevels = 1,
        const std::uint32_t arrayLayers = 1,
        VkMemoryPropertyFlags memoryPropertyFlags =
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL);

    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;

    Image(Image&& other) noexcept;
    Image& operator=(Image&& other);

    ~Image();
    void Destroy();

    void CopyBufferToImage(const Buffer& srcBuffer);
    void BlitToImage(const CommandBuffer& commandBuffer, const Image& dstImage);

    void Update(void* data, std::size_t dataSize);

    void SaveToFile(const std::filesystem::path& filePath);

    static Image LoadFromFiles(
        std::string_view name,
        ImageType type,
        std::span<std::string_view> files,
        bool mipmapped = false);

    static Image LoadFromFile(
        std::string_view name,
        const std::string& filePath,
        bool mipmapped = false);

    void SetLayout(const CommandBuffer& commandBuffer, VkImageLayout layout);

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
    std::uint32_t GetArrayLayers() const;
    VkImageAspectFlags GetAspectMask() const;

private:
    void CreateImage(
        VkMemoryPropertyFlags memoryPropertyFlags,
        VkImageTiling tiling);
    void CreateImageView();
    void GenerateMipmaps(VkCommandBuffer commandBuffer);

private:
    VkImage m_handle{ VK_NULL_HANDLE };
    VkImageView m_view{ VK_NULL_HANDLE };
    VmaAllocation m_allocation{ VK_NULL_HANDLE };
    VmaAllocationInfo m_info{};

    std::string_view m_name;

    ImageType m_type;
    VkExtent3D m_extent;
    VkFormat m_format;
    VkImageLayout m_layout;
    std::uint32_t m_mipLevels;
    std::uint32_t m_arrayLayers;
    VkImageTiling m_tiling;
    VkImageUsageFlags m_usage;
    VkMemoryPropertyFlags m_memoryPropertyFlags;
    VkImageAspectFlags m_aspectMask;
};
}
