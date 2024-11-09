#include "DescriptorSetLayout.hpp"

#include "Definitions.hpp"
#include "Descriptor.hpp"
#include "VkContext.hpp"
#include "api/vulkan_debug.hpp"

#include <vulkan/vulkan_core.h>

#include <vector>

namespace Zeus
{
DescriptorSetLayout::DescriptorSetLayout(
    const std::vector<Descriptor>& descriptors,
    const std::vector<PushConstants>& pushConstants,
    const char* name)
    : m_descriptors{ descriptors },
      m_pushConstants{ pushConstants },
      m_name{ name }
{
    Build();
}

DescriptorSetLayout::~DescriptorSetLayout()
{
    if (m_handle == VK_NULL_HANDLE)
        return;

    VkContext::GetDevice().GetDeletionQueue().Add(
        ResourceType::DescriptorSetLayout,
        m_handle);
    m_handle = VK_NULL_HANDLE;
    m_name = nullptr;
}

void DescriptorSetLayout::AddDescriptor(
    VkDescriptorType type,
    VkShaderStageFlags stageFlags,
    std::uint32_t binding)
{
    m_descriptors.emplace_back(Descriptor(type, stageFlags, binding));
}

void DescriptorSetLayout::AddPushConstants(
    VkShaderStageFlags stageFlags,
    std::uint32_t offset,
    std::uint32_t size)
{
    m_pushConstants.emplace_back(PushConstants(stageFlags, offset, size));
}

void DescriptorSetLayout::Build()
{
    assert(m_handle == VK_NULL_HANDLE && "DescriptorSetLayout already built.");

    std::vector<VkDescriptorSetLayoutBinding> layoutBindings{};

    for (const Descriptor& descriptor : m_descriptors)
    {
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = descriptor.GetBinding();
        layoutBinding.descriptorType = descriptor.GetType();
        layoutBinding.descriptorCount = 1;
        layoutBinding.stageFlags = descriptor.GetStageFlags();
        layoutBinding.pImmutableSamplers = nullptr;
        layoutBindings.emplace_back(layoutBinding);
    }

    VkDescriptorSetLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.flags = 0;
    createInfo.bindingCount = static_cast<std::uint32_t>(layoutBindings.size());
    createInfo.pBindings = layoutBindings.data();

    VKCHECK(
        vkCreateDescriptorSetLayout(
            VkContext::GetLogicalDevice(),
            &createInfo,
            allocationCallbacks.get(),
            &m_handle),
        "Failed to create descriptor set layout.");

#ifndef NDEBUG
    if (m_name != nullptr)
    {
        setDebugUtilsObjectNameEXT(
            VkContext::GetLogicalDevice(),
            VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT,
            reinterpret_cast<std::uint64_t>(m_handle),
            m_name);
    }
#endif
}

const VkDescriptorSetLayout& DescriptorSetLayout::GetHandle() const
{
    return m_handle;
}

const std::vector<Descriptor>& DescriptorSetLayout::GetDescriptors() const
{
    return m_descriptors;
}

const std::vector<PushConstants>& DescriptorSetLayout::GetPushConstants() const
{
    return m_pushConstants;
}
}