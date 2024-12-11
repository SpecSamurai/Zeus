#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>

namespace Zeus
{
class Descriptor
{
public:
    Descriptor(
        VkDescriptorType type,
        VkShaderStageFlags stageFlags,
        std::uint32_t binding);

    VkDescriptorType GetType() const;
    VkShaderStageFlags GetStageFlags() const;
    std::uint32_t GetBinding() const;

private:
    VkDescriptorType m_type;
    VkShaderStageFlags m_stageFlags;
    std::uint32_t m_binding;
};

class DescriptorBuffer : public Descriptor
{
public:
    DescriptorBuffer(
        VkDescriptorType type,
        VkShaderStageFlags stageFlags,
        std::uint32_t binding,
        VkBuffer buffer,
        VkDeviceSize range,
        VkDeviceSize offset = 0);

    VkBuffer GetBuffer() const;
    VkDeviceSize GetRange() const;
    VkDeviceSize GetOffset() const;

private:
    VkBuffer m_buffer{ VK_NULL_HANDLE };
    VkDeviceSize m_range;
    VkDeviceSize m_offset;
};

class DescriptorImage : public Descriptor
{
public:
    DescriptorImage(
        VkDescriptorType type,
        VkShaderStageFlags stageFlags,
        std::uint32_t binding,
        VkImageView imageView = VK_NULL_HANDLE,
        VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        VkSampler sampler = VK_NULL_HANDLE);

    VkSampler GetSampler() const;
    VkImageView GetImageView() const;
    VkImageLayout GetImageLayout() const;

private:
    VkSampler m_sampler{ VK_NULL_HANDLE };
    VkImageView m_imageView{ VK_NULL_HANDLE };
    VkImageLayout m_imageLayout{ VK_IMAGE_LAYOUT_UNDEFINED };
};
}
