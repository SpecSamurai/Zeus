#include "DescriptorSet.hpp"

#include "DescriptorPool.hpp"
#include "DescriptorSetLayout.hpp"
#include "VkContext.hpp"

#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cstdint>
#include <string_view>
#include <vector>

namespace Zeus
{
DescriptorSet::DescriptorSet(
    std::string_view name,
    const DescriptorSetLayout& descriptorSetLayout,
    DescriptorPool& descriptorPool)
    : m_name{ name }
{
    m_handle = descriptorPool.Allocate(descriptorSetLayout.GetHandle());

    VkContext::SetDebugName(VK_OBJECT_TYPE_DESCRIPTOR_SET, m_handle, m_name);
}

DescriptorSet::DescriptorSet(DescriptorSet&& other) noexcept
    : m_handle{ other.m_handle },
      m_name{ other.m_name }
{
    other.m_handle = VK_NULL_HANDLE;
}

DescriptorSet& DescriptorSet::operator=(DescriptorSet&& other)
{
    if (this != &other)
    {
        if (m_handle != VK_NULL_HANDLE)
        {
            // Free();
        }

        m_handle = other.m_handle;
        m_name = other.m_name;

        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}

void DescriptorSet::Update(
    const std::vector<VkDescriptorBufferInfo>& bufferInfos,
    std::uint32_t dstBinding,
    VkDescriptorType descriptorType,
    std::uint32_t dstArrayElement)
{
    VkWriteDescriptorSet writeDescriptorSet{};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.pNext = nullptr;
    writeDescriptorSet.dstSet = m_handle;
    writeDescriptorSet.dstBinding = dstBinding;
    writeDescriptorSet.dstArrayElement = dstArrayElement;
    writeDescriptorSet.descriptorCount =
        static_cast<std::uint32_t>(bufferInfos.size());
    writeDescriptorSet.descriptorType = descriptorType;
    writeDescriptorSet.pBufferInfo = bufferInfos.data();

    vkUpdateDescriptorSets(
        VkContext::LogicalDevice(),
        1,
        &writeDescriptorSet,
        0,
        nullptr);
}

void DescriptorSet::Update(
    const std::vector<VkDescriptorImageInfo>& imageInfos,
    std::uint32_t dstBinding,
    VkDescriptorType descriptorType,
    std::uint32_t dstArrayElement)
{
    VkWriteDescriptorSet writeDescriptorSet{};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.pNext = nullptr;
    writeDescriptorSet.dstSet = m_handle;
    writeDescriptorSet.dstBinding = dstBinding;
    writeDescriptorSet.dstArrayElement = dstArrayElement;
    writeDescriptorSet.descriptorCount =
        static_cast<std::uint32_t>(imageInfos.size());
    writeDescriptorSet.descriptorType = descriptorType;
    writeDescriptorSet.pImageInfo = imageInfos.data();

    vkUpdateDescriptorSets(
        VkContext::LogicalDevice(),
        1,
        &writeDescriptorSet,
        0,
        nullptr);
}

VkDescriptorSet DescriptorSet::GetHandle() const
{
    return m_handle;
}
}
