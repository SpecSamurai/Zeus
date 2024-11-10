#pragma once

#include "Descriptor.hpp"

#include <vulkan/vulkan_core.h>

#include <vector>

namespace Zeus
{
class DescriptorSetLayout
{
public:
    DescriptorSetLayout() = default;

    DescriptorSetLayout(
        const std::vector<Descriptor>& descriptors,
        const char* name = nullptr);

    ~DescriptorSetLayout();

    void AddDescriptor(
        VkDescriptorType type,
        VkShaderStageFlags stageFlags,
        std::uint32_t binding);

    void Build();

    const VkDescriptorSetLayout& GetHandle() const;
    const std::vector<Descriptor>& GetDescriptors() const;

private:
    VkDescriptorSetLayout m_handle{ VK_NULL_HANDLE };
    std::vector<Descriptor> m_descriptors;

    const char* m_name{ nullptr };
};
}
