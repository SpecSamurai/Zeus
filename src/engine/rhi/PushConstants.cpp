#include "PushConstants.hpp"

#include "VkContext.hpp"

namespace Zeus
{
PushConstants::PushConstants(
    VkShaderStageFlags stageFlags,
    std::uint32_t offset,
    std::uint32_t size)
    : m_stageFlags{ stageFlags },
      m_offset{ offset },
      m_size{ size }
{
    assert(m_size <= VkContext::Device().GetMaxPushConstantsSize());
}

VkShaderStageFlags PushConstants::GetStageFlags() const
{
    return m_stageFlags;
}

std::uint32_t PushConstants::GetOffset() const
{
    return m_offset;
}

std::uint32_t PushConstants::GetSize() const
{
    return m_size;
}
}
