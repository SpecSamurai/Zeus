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
}
