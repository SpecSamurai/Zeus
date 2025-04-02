#include "DescriptorSet.hpp"

#include "Descriptor.hpp"
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
    DescriptorSetLayout& descriptorSetLayout,
    DescriptorPool& descriptorPool)
    : m_name{ name }
{
    m_handle = descriptorPool.Allocate(descriptorSetLayout.GetHandle());

    VkContext::SetDebugName(VK_OBJECT_TYPE_DESCRIPTOR_SET, m_handle, m_name);
}

DescriptorSet::DescriptorSet(DescriptorSet&& other) noexcept
    : m_handle{ other.m_handle },
      m_binding{ other.m_binding },
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
        m_binding = other.m_binding;
        m_name = other.m_name;

        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}

void DescriptorSet::Update(
    const std::vector<DescriptorBuffer>& bufferDescriptors,
    const std::vector<DescriptorImage>& imageDescriptors)
{
    std::vector<VkWriteDescriptorSet> descriptorSetWrites{};
    descriptorSetWrites.reserve(
        bufferDescriptors.size() + imageDescriptors.size());

    std::vector<VkDescriptorBufferInfo> bufferInfos;
    bufferInfos.reserve(bufferDescriptors.size());

    for (const DescriptorBuffer& descriptor : bufferDescriptors)
    {
        bufferInfos.emplace_back(VkDescriptorBufferInfo{
            .buffer = descriptor.GetBuffer(),
            .offset = descriptor.GetOffset(),
            .range = descriptor.GetRange(),
        });

        VkWriteDescriptorSet writeDescriptorSet{};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.pNext = nullptr;
        writeDescriptorSet.dstSet = m_handle;
        writeDescriptorSet.dstBinding = descriptor.GetBinding();
        writeDescriptorSet.dstArrayElement = 0;
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.descriptorType = descriptor.GetType();
        writeDescriptorSet.pBufferInfo = bufferInfos.data();

        descriptorSetWrites.emplace_back(writeDescriptorSet);
    }

    std::vector<VkDescriptorImageInfo> imageInfos;
    imageInfos.reserve(imageDescriptors.size());

    for (const DescriptorImage& descriptor : imageDescriptors)
    {
        VkDescriptorImageInfo& info =
            imageInfos.emplace_back(VkDescriptorImageInfo{
                .sampler = descriptor.GetSampler(),
                .imageView = descriptor.GetImageView(),
                .imageLayout = descriptor.GetImageLayout(),
            });

        VkWriteDescriptorSet writeDescriptorSet{};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.pNext = nullptr;
        writeDescriptorSet.dstSet = m_handle;
        writeDescriptorSet.dstBinding = descriptor.GetBinding();
        writeDescriptorSet.dstArrayElement = 0;
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.descriptorType = descriptor.GetType();
        writeDescriptorSet.pImageInfo = &info;

        descriptorSetWrites.push_back(writeDescriptorSet);
    }

    vkUpdateDescriptorSets(
        VkContext::GetLogicalDevice(),
        static_cast<std::uint32_t>(descriptorSetWrites.size()),
        descriptorSetWrites.data(),
        0,
        nullptr);
}

VkDescriptorSet DescriptorSet::GetHandle() const
{
    return m_handle;
}
}
