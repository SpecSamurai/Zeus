#include "DescriptorLayoutBuilder.hpp"

#include "vulkan_debug.hpp"
#include "vulkan_memory.hpp"

#include <vulkan/vulkan.h>

namespace Zeus
{
VkDescriptorSetLayout DescriptorLayoutBuilder::build(
    VkDevice device,
    VkDescriptorSetLayoutCreateFlags flags)
{
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSetLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.flags = flags;
    createInfo.bindingCount = static_cast<std::uint32_t>(layoutBindings.size());
    createInfo.pBindings = layoutBindings.data();

    VkResult result{ vkCreateDescriptorSetLayout(
        device,
        &createInfo,
        allocationCallbacks.get(),
        &descriptorSetLayout) };

    VKCHECK(result, "Failed to create descriptor set layout.");

    return descriptorSetLayout;
}

void DescriptorLayoutBuilder::addBinding(
    std::uint32_t binding,
    VkDescriptorType descriptorType,
    VkShaderStageFlags stageFlags,
    std::uint32_t descriptorCount,
    const VkSampler* pImmutableSamplers)
{
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = binding;
    layoutBinding.descriptorType = descriptorType;
    layoutBinding.descriptorCount = descriptorCount;
    layoutBinding.stageFlags = stageFlags;
    layoutBinding.pImmutableSamplers = pImmutableSamplers;

    layoutBindings.emplace_back(layoutBinding);
}

void DescriptorLayoutBuilder::clear()
{
    layoutBindings.clear();
}
}
