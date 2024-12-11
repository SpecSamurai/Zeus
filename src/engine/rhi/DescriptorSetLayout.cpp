#include "DescriptorSetLayout.hpp"

#include "Definitions.hpp"
#include "Descriptor.hpp"
#include "VkContext.hpp"
#include "vulkan/vulkan_debug.hpp"

#include <vulkan/vulkan_core.h>

#include <string_view>
#include <vector>

namespace Zeus
{
DescriptorSetLayout::DescriptorSetLayout(
    const std::vector<Descriptor>& descriptors,
    std::string_view name)
    : m_descriptors{ descriptors }
{
    std::vector<VkDescriptorSetLayoutBinding> layoutBindings{};
    layoutBindings.reserve(m_descriptors.size());

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

    VkContext::SetDebugName(
        VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT,
        m_handle,
        name);
}

DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& other) noexcept
    : m_handle{ other.m_handle },
      m_descriptors{ other.m_descriptors }
{
    other.m_handle = VK_NULL_HANDLE;
    other.m_descriptors.clear();
}

DescriptorSetLayout& DescriptorSetLayout::operator=(DescriptorSetLayout&& other)
{
    if (this != &other)
    {
        if (m_handle != VK_NULL_HANDLE)
        {
            Destroy();
        }

        m_handle = other.m_handle;
        m_descriptors = std::move(other.m_descriptors);

        other.m_handle = VK_NULL_HANDLE;
        other.m_descriptors.clear();
    }

    return *this;
}

DescriptorSetLayout::~DescriptorSetLayout()
{
    if (m_handle == VK_NULL_HANDLE)
        return;

    VkContext::GetDeletionQueue().Add(
        ResourceType::DescriptorSetLayout,
        m_handle);
    m_handle = VK_NULL_HANDLE;
    m_descriptors.clear();
}

void DescriptorSetLayout::Destroy()
{
    vkDestroyDescriptorSetLayout(
        VkContext::GetLogicalDevice(),
        m_handle,
        allocationCallbacks.get());
    m_handle = VK_NULL_HANDLE;
}

VkDescriptorSetLayout DescriptorSetLayout::GetHandle() const
{
    return m_handle;
}

const std::vector<Descriptor>& DescriptorSetLayout::GetDescriptors() const
{
    return m_descriptors;
}
}
