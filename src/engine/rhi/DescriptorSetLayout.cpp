#include "DescriptorSetLayout.hpp"

#include "Definitions.hpp"
#include "VkContext.hpp"
#include "vulkan/vulkan_debug.hpp"

#include <vulkan/vulkan_core.h>

#include <string_view>
#include <vector>

namespace Zeus
{
DescriptorSetLayout::DescriptorSetLayout(
    std::string_view name,
    const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings,
    VkDescriptorSetLayoutCreateFlags flags,
    const std::vector<VkDescriptorBindingFlags>& bindingFlags)
    : m_name{ name }
{
    VkDescriptorSetLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.flags = flags;
    createInfo.bindingCount = static_cast<std::uint32_t>(layoutBindings.size());
    createInfo.pBindings = layoutBindings.data();
    createInfo.pNext = nullptr;

    VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsCreateInfo{};
    if (!bindingFlags.empty())
    {
        bindingFlagsCreateInfo.sType =
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
        bindingFlagsCreateInfo.bindingCount =
            static_cast<std::uint32_t>(bindingFlags.size());
        bindingFlagsCreateInfo.pBindingFlags = bindingFlags.data();

        createInfo.pNext = &bindingFlagsCreateInfo;
    }

    VKCHECK(
        vkCreateDescriptorSetLayout(
            VkContext::LogicalDevice(),
            &createInfo,
            allocationCallbacks.get(),
            &m_handle),
        "Failed to create descriptor set layout.");

    VkContext::SetDebugName(
        VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT,
        m_handle,
        m_name);
}

DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& other) noexcept
    : m_handle{ other.m_handle },
      m_name{ other.m_name }
{
    other.m_handle = VK_NULL_HANDLE;
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
        m_name = other.m_name;

        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}

DescriptorSetLayout::~DescriptorSetLayout()
{
    if (m_handle == VK_NULL_HANDLE)
        return;

    VkContext::DeletionQueue().Add(ResourceType::DescriptorSetLayout, m_handle);
    m_handle = VK_NULL_HANDLE;
}

void DescriptorSetLayout::Destroy()
{
    vkDestroyDescriptorSetLayout(
        VkContext::LogicalDevice(),
        m_handle,
        allocationCallbacks.get());

    m_handle = VK_NULL_HANDLE;
}

VkDescriptorSetLayout DescriptorSetLayout::GetHandle() const
{
    return m_handle;
}
}
