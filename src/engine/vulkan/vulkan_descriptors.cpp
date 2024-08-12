#include "vulkan_descriptors.hpp"

#include "vulkan_utils.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
VkResult createDescriptorPool(
    VkDescriptorPool& descriptorPool,
    const VkDevice& device,
    std::uint32_t maxSets,
    std::uint32_t poolSizeCount,
    const VkDescriptorPoolSize* pPoolSizes,
    VkDescriptorPoolCreateFlags flags)
{
    VkDescriptorPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.flags = flags;
    createInfo.maxSets = maxSets;
    createInfo.poolSizeCount = poolSizeCount;
    createInfo.pPoolSizes = pPoolSizes;

    VkResult result{
        vkCreateDescriptorPool(device, &createInfo, nullptr, &descriptorPool)
    };

    VKCHECK(result, "Failed to create descriptor pool.");

    return result;
}

VkResult createDescriptorSetLayout(
    VkDescriptorSetLayout& descriptorSetLayout,
    VkDevice device,
    std::uint32_t bindingCount,
    const VkDescriptorSetLayoutBinding* pBindings,
    VkDescriptorSetLayoutCreateFlags flags)
{
    VkDescriptorSetLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.flags = flags;
    createInfo.bindingCount = bindingCount;
    createInfo.pBindings = pBindings;

    VkResult result{ vkCreateDescriptorSetLayout(
        device,
        &createInfo,
        nullptr,
        &descriptorSetLayout) };

    VKCHECK(result, "Failed to create descriptor set layout.");

    return result;
}

std::vector<VkDescriptorSet> allocateVkDescriptorSets(
    VkDevice device,
    VkDescriptorPool descriptorPool,
    VkDescriptorSetLayout descriptorSetLayout,
    std::uint32_t descriptorSetCount)
{
    std::vector<VkDescriptorSet> descriptorSets(descriptorSetCount);

    std::vector<VkDescriptorSetLayout> layouts(
        descriptorSetCount,
        descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = descriptorPool;
    allocateInfo.descriptorSetCount = descriptorSetCount;
    allocateInfo.pSetLayouts = layouts.data();

    VkResult result{
        vkAllocateDescriptorSets(device, &allocateInfo, descriptorSets.data())
    };

    VKCHECK(result, "Failed to allocate descriptor sets");

    return descriptorSets;
}

VkDescriptorSetLayoutBinding createVkDescriptorSetLayoutBinding(
    std::uint32_t binding,
    VkDescriptorType descriptorType,
    std::uint32_t descriptorCount,
    VkShaderStageFlags stageFlags,
    const VkSampler* pImmutableSamplers)
{
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = binding;
    layoutBinding.descriptorType = descriptorType;
    layoutBinding.descriptorCount = descriptorCount;
    layoutBinding.stageFlags = stageFlags;
    layoutBinding.pImmutableSamplers = pImmutableSamplers;

    return layoutBinding;
}

VkDescriptorPoolSize createVkDescriptorPoolSize(
    VkDescriptorType type,
    std::uint32_t descriptorCount)
{
    VkDescriptorPoolSize poolSize{};
    poolSize.type = type;
    poolSize.descriptorCount = descriptorCount;

    return poolSize;
}

VkWriteDescriptorSet addWriteDescriptorSet(
    VkDescriptorSet dstSet,
    std::uint32_t dstBinding,
    std::uint32_t dstArrayElement,
    std::uint32_t descriptorCount,
    VkDescriptorType descriptorType,
    const VkDescriptorImageInfo* pImageInfo,
    const VkDescriptorBufferInfo* pBufferInfo,
    const VkBufferView* pTexelBufferView)
{
    VkWriteDescriptorSet writeDescriptorSet{};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

    writeDescriptorSet.dstSet = dstSet;
    writeDescriptorSet.dstBinding = dstBinding;
    writeDescriptorSet.dstArrayElement = dstArrayElement;
    writeDescriptorSet.descriptorCount = descriptorCount;
    writeDescriptorSet.descriptorType = descriptorType;
    writeDescriptorSet.pImageInfo = pImageInfo;
    writeDescriptorSet.pBufferInfo = pBufferInfo;
    writeDescriptorSet.pTexelBufferView = pTexelBufferView;

    return writeDescriptorSet;
}
}
