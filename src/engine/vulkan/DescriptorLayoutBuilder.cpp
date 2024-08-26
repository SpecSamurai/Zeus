#include "DescriptorLayoutBuilder.hpp"

#include "vulkan_descriptors.hpp"

#include <vulkan/vulkan.h>

namespace Zeus
{
VkDescriptorSetLayout DescriptorLayoutBuilder::build(
    VkDevice device,
    VkDescriptorSetLayoutCreateFlags flags)
{
    VkDescriptorSetLayout descriptorSetLayout;

    createDescriptorSetLayout(
        descriptorSetLayout,
        device,
        static_cast<std::uint32_t>(layoutBindings.size()),
        layoutBindings.data(),
        flags);

    return descriptorSetLayout;
}

void DescriptorLayoutBuilder::addBinding(
    std::uint32_t binding,
    VkDescriptorType descriptorType,
    VkShaderStageFlags stageFlags,
    std::uint32_t descriptorCount,
    const VkSampler* pImmutableSamplers)
{
    VkDescriptorSetLayoutBinding layoutBinding{
        createVkDescriptorSetLayoutBinding(
            binding,
            descriptorType,
            descriptorCount,
            stageFlags,
            pImmutableSamplers)
    };

    layoutBindings.push_back(layoutBinding);
}

void DescriptorLayoutBuilder::clear()
{
    layoutBindings.clear();
}
}
