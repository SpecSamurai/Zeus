#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>

namespace Zeus
{
class PushConstants
{
public:
    PushConstants(
        VkShaderStageFlags stageFlags,
        std::uint32_t offset,
        std::uint32_t size);

    VkShaderStageFlags GetStageFlags() const;
    std::uint32_t GetOffset() const;
    std::uint32_t GetSize() const;

private:
    VkShaderStageFlags m_stageFlags;
    std::uint32_t m_offset;
    std::uint32_t m_size;
};
}
