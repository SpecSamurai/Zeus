#include "DynamicDescriptorAllocator.hpp"

#include "api/vulkan_debug.hpp"
#include "api/vulkan_descriptors.hpp"
#include "api/vulkan_memory.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
void DynamicDescriptorAllocator::Init(
    VkDevice device,
    std::uint32_t maxSetsPerPool,
    const std::vector<VkDescriptorPoolSize>& poolSizes,
    VkDescriptorPoolCreateFlags flags)
{
    m_flags = flags;
    m_maxSetsPerPool = maxSetsPerPool;
    m_poolSizes.clear();

    for (auto poolSize : poolSizes)
    {
        m_poolSizes.push_back(poolSize);
    }

    VkDescriptorPool descriptorPool;
    createDescriptorPool(
        &descriptorPool,
        device,
        m_maxSetsPerPool,
        static_cast<std::uint32_t>(m_poolSizes.size()),
        m_poolSizes.data(),
        m_flags);

    m_pools.push_back(descriptorPool);
}

VkDescriptorSet DynamicDescriptorAllocator::Allocate(
    VkDevice device,
    VkDescriptorSetLayout descriptorSetLayout)
{
    VkDescriptorPool descriptorPool{ GetOrCreatePool(device) };

    VkDescriptorSet descriptorSet;
    VkResult result{ allocateVkDescriptorSet(
        &descriptorSet,
        device,
        descriptorPool,
        descriptorSetLayout) };

    if (result == VK_ERROR_OUT_OF_POOL_MEMORY ||
        result == VK_ERROR_FRAGMENTED_POOL)
    {
        m_fullPools.push_back(descriptorPool);

        descriptorPool = GetOrCreatePool(device);

        VKCHECK(
            allocateVkDescriptorSet(
                &descriptorSet,
                device,
                descriptorPool,
                descriptorSetLayout),
            "Failed to allocate descriptor sets");
    }

    m_pools.push_back(descriptorPool);
    return descriptorSet;
}

void DynamicDescriptorAllocator::Clear(
    VkDevice device,
    VkDescriptorPoolResetFlags flags)
{
    for (auto pool : m_pools)
    {
        vkResetDescriptorPool(device, pool, flags);
    }

    for (auto pool : m_fullPools)
    {
        vkResetDescriptorPool(device, pool, flags);
        m_pools.push_back(pool);
    }

    m_fullPools.clear();
}

void DynamicDescriptorAllocator::Destroy(VkDevice device)
{
    for (auto pool : m_pools)
    {
        vkDestroyDescriptorPool(device, pool, allocationCallbacks.get());
    }
    m_pools.clear();

    for (auto pool : m_fullPools)
    {
        vkDestroyDescriptorPool(device, pool, allocationCallbacks.get());
    }
    m_fullPools.clear();
}

VkDescriptorPool DynamicDescriptorAllocator::GetOrCreatePool(VkDevice device)
{
    VkDescriptorPool descriptorPool;

    if (m_pools.size() != 0)
    {
        descriptorPool = m_pools.back();
        m_pools.pop_back();
    }
    else
    {
        createDescriptorPool(
            &descriptorPool,
            device,
            m_maxSetsPerPool,
            static_cast<std::uint32_t>(m_poolSizes.size()),
            m_poolSizes.data(),
            m_flags);
    }

    return descriptorPool;
}
}
