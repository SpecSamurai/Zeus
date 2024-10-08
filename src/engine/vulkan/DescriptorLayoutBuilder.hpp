#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Zeus
{
class DescriptorLayoutBuilder
{
public:
    VkDescriptorSetLayout Build(
        VkDevice device,
        VkDescriptorSetLayoutCreateFlags flags = 0);

    void AddBinding(
        std::uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        std::uint32_t descriptorCount = 1,
        const VkSampler* pImmutableSamplers = nullptr);

    void Clear();

private:
    std::vector<VkDescriptorSetLayoutBinding> m_layoutBindings{};
};
}
