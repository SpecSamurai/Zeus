#include "DescriptorPool.hpp"

#include "VkContext.hpp"
#include "api/vulkan_debug.hpp"
#include "api/vulkan_descriptors.hpp"

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

#ifndef NDEBUG
    if (!m_name.empty())
    {
        setDebugUtilsObjectNameEXT(
            VkContext::GetLogicalDevice(),
            VK_OBJECT_TYPE_DESCRIPTOR_POOL,
            reinterpret_cast<std::uint64_t>(m_handle),
            m_name.c_str());
    }
#endif
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
