#include "DescriptorPool.hpp"

#include "VkContext.hpp"
#include "rhi/vulkan_debug.hpp"
#include "rhi/vulkan_descriptors.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <string>
#include <vector>

namespace Zeus
{
DescriptorPool::DescriptorPool(const char* name) : m_name(name)
{
}

DescriptorPool::~DescriptorPool()
{
    if (m_handle == VK_NULL_HANDLE)
        return;

    VkContext::GetDeletionQueue().Add(ResourceType::DescriptorPool, m_handle);
    m_handle = VK_NULL_HANDLE;
}

void DescriptorPool::Destroy()
{
    vkDestroyDescriptorPool(
        VkContext::GetLogicalDevice(),
        m_handle,
        allocationCallbacks.get());
    m_handle = VK_NULL_HANDLE;
}

VkDescriptorSet DescriptorPool::Allocate(
    VkDescriptorSetLayout descriptorSetLayout)
{
    VkDescriptorSet descriptorSet;

    allocateVkDescriptorSet(
        &descriptorSet,
        VkContext::GetLogicalDevice(),
        m_handle,
        descriptorSetLayout);

    return descriptorSet;
}

void DescriptorPool::Init(
    std::uint32_t maxSets,
    const std::vector<VkDescriptorPoolSize>& poolSizes,
    VkDescriptorPoolCreateFlags flags)
{
    createDescriptorPool(
        &m_handle,
        VkContext::GetLogicalDevice(),
        maxSets,
        static_cast<std::uint32_t>(poolSizes.size()),
        poolSizes.data(),
        flags);

    VkContext::SetDebugName(VK_OBJECT_TYPE_DESCRIPTOR_POOL, m_handle, m_name);
}

void DescriptorPool::Reset()
{
    VKCHECK(
        vkResetDescriptorPool(VkContext::GetLogicalDevice(), m_handle, 0),
        "Failed to reset descriptor pool.");
}

VkDescriptorPool DescriptorPool::GetHandle() const
{
    return m_handle;
}
}
