#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Zeus
{
class DescriptorLayoutBuilder
{
public:
    VkDescriptorSetLayout build(
        VkDevice device,
        VkDescriptorSetLayoutCreateFlags flags = 0);

    void addBinding(
        std::uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        std::uint32_t descriptorCount = 1,
        const VkSampler* pImmutableSamplers = nullptr);

    void clear();

private:
    std::vector<VkDescriptorSetLayoutBinding> layoutBindings{};
};
}
