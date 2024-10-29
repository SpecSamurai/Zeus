#include "DescriptorAllocator.hpp"

#include "api/vulkan_descriptors.hpp"
#include "api/vulkan_memory.hpp"

#include <cstdint>
#include <vector>

namespace Zeus
{
VkDescriptorSet DescriptorAllocator::allocate(
    VkDevice device,
    VkDescriptorSetLayout descriptorSetLayout)
{
    VkDescriptorSet descriptorSet;

    allocateVkDescriptorSet(
        &descriptorSet,
        device,
        descriptorPool,
        descriptorSetLayout);

    return descriptorSet;
}

void DescriptorAllocator::init(
    VkDevice device,
    std::uint32_t maxSets,
    const std::vector<VkDescriptorPoolSize>& poolSizes,
    VkDescriptorPoolCreateFlags flags)
{
    createDescriptorPool(
        &descriptorPool,
        device,
        maxSets,
        static_cast<std::uint32_t>(poolSizes.size()),
        poolSizes.data(),
        flags);
}

void DescriptorAllocator::clearDescriptors(VkDevice device)
{
    vkResetDescriptorPool(device, descriptorPool, 0);
}

void DescriptorAllocator::destroyPool(VkDevice device)
{
    vkDestroyDescriptorPool(device, descriptorPool, allocationCallbacks.get());
}
}
