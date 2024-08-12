#pragma once

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
    VkDescriptorPoolCreateFlags flags = 0);

VkResult createDescriptorSetLayout(
    VkDescriptorSetLayout& descriptorSetLayout,
    VkDevice device,
    std::uint32_t bindingCount,
    const VkDescriptorSetLayoutBinding* pBindings,
    VkDescriptorSetLayoutCreateFlags flags = 0);

std::vector<VkDescriptorSet> allocateVkDescriptorSets(
    VkDevice device,
    VkDescriptorPool descriptorPool,
    VkDescriptorSetLayout descriptorSetLayout,
    std::uint32_t descriptorSetCount);

VkDescriptorSetLayoutBinding createVkDescriptorSetLayoutBinding(
    std::uint32_t binding,
    VkDescriptorType descriptorType,
    std::uint32_t descriptorCount,
    VkShaderStageFlags stageFlags,
    const VkSampler* pImmutableSamplers = nullptr);

VkDescriptorPoolSize createVkDescriptorPoolSize(
    VkDescriptorType type,
    std::uint32_t descriptorCount);

VkWriteDescriptorSet addWriteDescriptorSet(
    VkDescriptorSet dstSet,
    std::uint32_t dstBinding,
    std::uint32_t dstArrayElement,
    std::uint32_t descriptorCount,
    VkDescriptorType descriptorType,
    const VkDescriptorImageInfo* pImageInfo = nullptr,
    const VkDescriptorBufferInfo* pBufferInfo = nullptr,
    const VkBufferView* pTexelBufferView = nullptr);
}
