#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
class VertexInputAttribute
{
public:
    VertexInputAttribute(
        std::uint32_t location,
        std::uint32_t binding,
        std::uint32_t offset,
        VkFormat format);

    std::uint32_t GetLocation() const;
    std::uint32_t GetBinding() const;
    std::uint32_t GetOffset() const;
    VkFormat GetFormat() const;

private:
    std::uint32_t m_location;
    std::uint32_t m_binding;
    std::uint32_t m_offset;
    VkFormat m_format;
};

class VertexInput
{
public:
    VertexInput(
        std::uint32_t binding,
        std::uint32_t stride,
        VkVertexInputRate inputRate,
        const std::vector<VertexInputAttribute>& attributes);

    std::uint32_t GetBinding() const;
    std::uint32_t GetStride() const;
    VkVertexInputRate GetInputRate() const;
    const std::vector<VertexInputAttribute>& GetAttributes() const;

private:
    std::uint32_t m_binding;
    std::uint32_t m_stride;
    VkVertexInputRate m_inputRate;
    std::vector<VertexInputAttribute> m_attributes;
};
}
