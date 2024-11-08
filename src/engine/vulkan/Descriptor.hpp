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
}
