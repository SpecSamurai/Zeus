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

VkResult allocateVkDescriptorSets(
    std::vector<VkDescriptorSet>& descriptorSets,
    VkDevice device,
    VkDescriptorPool descriptorPool,
    VkDescriptorSetLayout descriptorSetLayout,
    std::uint32_t descriptorSetCount);

VkResult allocateVkDescriptorSet(
    VkDescriptorSet& descriptorSet,
    VkDevice device,
    VkDescriptorPool descriptorPool,
    VkDescriptorSetLayout descriptorSetLayout);

VkDescriptorPoolSize createVkDescriptorPoolSize(
    VkDescriptorType type,
    std::uint32_t descriptorCount);
}
