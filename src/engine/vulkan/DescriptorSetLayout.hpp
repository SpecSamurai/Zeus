#pragma once

#include "Descriptor.hpp"
#include "PushConstants.hpp"

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
        const std::vector<PushConstants>& pushConstants = {},
        const char* name = nullptr);

    ~DescriptorSetLayout();

    void AddDescriptor(
        VkDescriptorType type,
        VkShaderStageFlags stageFlags,
        std::uint32_t binding);

    void AddPushConstants(
        VkShaderStageFlags stageFlags,
        std::uint32_t offset,
        std::uint32_t size);

    void Build();

    const VkDescriptorSetLayout& GetHandle() const;
    const std::vector<Descriptor>& GetDescriptors() const;
    const std::vector<PushConstants>& GetPushConstants() const;

private:
    VkDescriptorSetLayout m_handle{ VK_NULL_HANDLE };
    std::vector<Descriptor> m_descriptors;
    std::vector<PushConstants> m_pushConstants;

    const char* m_name{ nullptr };
};
}
