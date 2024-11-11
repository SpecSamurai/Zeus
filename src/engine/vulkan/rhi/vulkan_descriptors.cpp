#include "vulkan_descriptors.hpp"

#include "vulkan_debug.hpp"
#include "vulkan_memory.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
VkResult createDescriptorPool(
    VkDescriptorPool* descriptorPool,
    VkDevice device,
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

    VkResult result{ vkCreateDescriptorPool(
        device,
        &createInfo,
        allocationCallbacks.get(),
        descriptorPool) };

    VKCHECK(result, "Failed to create descriptor pool.");

    return result;
}

VkResult allocateVkDescriptorSets(
    std::vector<VkDescriptorSet>& descriptorSets,
    VkDevice device,
    VkDescriptorPool descriptorPool,
    VkDescriptorSetLayout descriptorSetLayout,
    std::uint32_t descriptorSetCount)
{
    descriptorSets.resize(descriptorSetCount);

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

    return result;
}

VkResult allocateVkDescriptorSet(
    VkDescriptorSet* descriptorSet,
    VkDevice device,
    VkDescriptorPool descriptorPool,
    VkDescriptorSetLayout descriptorSetLayout)
{
    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = descriptorPool;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &descriptorSetLayout;

    return vkAllocateDescriptorSets(device, &allocateInfo, descriptorSet);
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
}
