#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
class DynamicDescriptorAllocator
{
public:
    void Init(
        VkDevice device,
        std::uint32_t maxSetsPerPool,
        const std::vector<VkDescriptorPoolSize>& poolSizes,
        VkDescriptorPoolCreateFlags flags = 0);

    VkDescriptorSet Allocate(
        VkDevice device,
        VkDescriptorSetLayout descriptorSetLayout);

    void Clear(VkDevice device, VkDescriptorPoolResetFlags flags = 0);

    void Destroy(VkDevice device);

private:
    VkDescriptorPool GetOrCreatePool(VkDevice device);

    VkDescriptorPoolCreateFlags m_flags{ 0 };
    std::uint32_t m_maxSetsPerPool;
    std::vector<VkDescriptorPoolSize> m_poolSizes;

    std::vector<VkDescriptorPool> m_pools;
    std::vector<VkDescriptorPool> m_fullPools;
};
}
