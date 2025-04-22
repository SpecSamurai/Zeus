#include "DescriptorPool.hpp"

#include "VkContext.hpp"
#include "vulkan/vulkan_debug.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <string_view>
#include <vector>

namespace Zeus
{
DescriptorPool::DescriptorPool(
    std::string_view name,
    std::uint32_t maxSets,
    const std::vector<VkDescriptorPoolSize>& poolSizes,
    VkDescriptorPoolCreateFlags flags)
    : m_name{ name }
{
    VkDescriptorPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.flags = flags;
    createInfo.maxSets = maxSets;
    createInfo.poolSizeCount = static_cast<std::uint32_t>(poolSizes.size());
    createInfo.pPoolSizes = poolSizes.data();

    VKCHECK(
        vkCreateDescriptorPool(
            VkContext::LogicalDevice(),
            &createInfo,
            allocationCallbacks.get(),
            &m_handle),
        "Failed to create descriptor pool.");

    VkContext::SetDebugName(VK_OBJECT_TYPE_DESCRIPTOR_POOL, m_handle, m_name);
}

DescriptorPool::~DescriptorPool()
{
    if (m_handle == VK_NULL_HANDLE)
        return;

    VkContext::DeletionQueue().Add(ResourceType::DescriptorPool, m_handle);
    m_handle = VK_NULL_HANDLE;
}

DescriptorPool::DescriptorPool(DescriptorPool&& other) noexcept
    : m_handle{ other.m_handle },
      m_name{ other.m_name }
{
    other.m_handle = VK_NULL_HANDLE;
}

DescriptorPool& DescriptorPool::operator=(DescriptorPool&& other)
{
    if (this != &other)
    {
        if (m_handle != VK_NULL_HANDLE)
        {
            Destroy();
        }

        m_handle = other.m_handle;
        m_name = other.m_name;

        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}

void DescriptorPool::Destroy()
{
    vkDestroyDescriptorPool(
        VkContext::LogicalDevice(),
        m_handle,
        allocationCallbacks.get());
    m_handle = VK_NULL_HANDLE;
}

VkDescriptorSet DescriptorPool::Allocate(
    VkDescriptorSetLayout descriptorSetLayout) const
{
    VkDescriptorSet descriptorSet;

    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = m_handle;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &descriptorSetLayout;

    VKCHECK(
        vkAllocateDescriptorSets(
            VkContext::LogicalDevice(),
            &allocateInfo,
            &descriptorSet),
        "Failed to allocate descriptor set.");

    return descriptorSet;
}

void DescriptorPool::Reset() const
{
    VKCHECK(
        vkResetDescriptorPool(VkContext::LogicalDevice(), m_handle, 0),
        "Failed to reset descriptor pool.");
}

VkDescriptorPool DescriptorPool::GetHandle() const
{
    return m_handle;
}
}
