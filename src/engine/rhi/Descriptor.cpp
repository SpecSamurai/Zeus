#include "Descriptor.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>

namespace Zeus
{
Descriptor::Descriptor(
    VkDescriptorType type,
    VkShaderStageFlags stageFlags,
    std::uint32_t binding)
    : m_type{ type },
      m_stageFlags{ stageFlags },
      m_binding{ binding }
{
}

VkDescriptorType Descriptor::GetType() const
{
    return m_type;
}

VkShaderStageFlags Descriptor::GetStageFlags() const
{
    return m_stageFlags;
}

std::uint32_t Descriptor::GetBinding() const
{
    return m_binding;
}

DescriptorBuffer::DescriptorBuffer(
    VkDescriptorType type,
    VkShaderStageFlags stageFlags,
    std::uint32_t binding,
    VkBuffer buffer,
    VkDeviceSize range,
    VkDeviceSize offset)
    : Descriptor(type, stageFlags, binding),
      m_buffer{ buffer },
      m_range{ range },
      m_offset{ offset }
{
}

VkBuffer DescriptorBuffer::GetBuffer() const
{
    return m_buffer;
}

VkDeviceSize DescriptorBuffer::GetRange() const
{
    return m_range;
}

VkDeviceSize DescriptorBuffer::GetOffset() const
{
    return m_offset;
}

DescriptorImage::DescriptorImage(
    VkDescriptorType type,
    VkShaderStageFlags stageFlags,
    std::uint32_t binding,
    VkImageView imageView,
    VkImageLayout imageLayout,
    VkSampler sampler)
    : Descriptor(type, stageFlags, binding),
      m_sampler{ sampler },
      m_imageView{ imageView },
      m_imageLayout{ imageLayout }
{
}

VkSampler DescriptorImage::GetSampler() const
{
    return m_sampler;
}

VkImageView DescriptorImage::GetImageView() const
{
    return m_imageView;
}

VkImageLayout DescriptorImage::GetImageLayout() const
{
    return m_imageLayout;
}
}
