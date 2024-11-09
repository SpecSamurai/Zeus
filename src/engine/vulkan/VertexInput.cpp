#include "VertexInput.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
VertexInputAttribute::VertexInputAttribute(
    std::uint32_t location,
    std::uint32_t binding,
    std::uint32_t offset,
    VkFormat format)
    : m_location{ location },
      m_binding{ binding },
      m_offset{ offset },
      m_format{ format }
{
}

std::uint32_t VertexInputAttribute::GetLocation() const
{
    return m_location;
}

std::uint32_t VertexInputAttribute::GetBinding() const
{
    return m_binding;
}

std::uint32_t VertexInputAttribute::GetOffset() const
{
    return m_offset;
}

VkFormat VertexInputAttribute::GetFormat() const
{
    return m_format;
}

VertexInput::VertexInput(
    std::uint32_t binding,
    std::uint32_t stride,
    VkVertexInputRate inputRate,
    const std::vector<VertexInputAttribute>& attributes)
    : m_binding{ binding },
      m_stride{ stride },
      m_inputRate{ inputRate },
      m_attributes{ attributes }
{
}

std::uint32_t VertexInput::GetBinding() const
{
    return m_binding;
}

std::uint32_t VertexInput::GetStride() const
{
    return m_stride;
}

VkVertexInputRate VertexInput::GetInputRate() const
{
    return m_inputRate;
}

const std::vector<VertexInputAttribute>& VertexInput::GetAttributes() const
{
    return m_attributes;
}
}
