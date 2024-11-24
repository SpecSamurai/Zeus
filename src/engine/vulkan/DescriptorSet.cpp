#include "DescriptorSet.hpp"

#include "Buffer.hpp"
#include "Descriptor.hpp"
#include "DescriptorPool.hpp"
#include "DescriptorSetLayout.hpp"
#include "Image.hpp"
#include "VkContext.hpp"

#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cstdint>
#include <vector>

namespace Zeus
{
DescriptorSet::DescriptorSet(
    const std::vector<Descriptor>& descriptors,
    DescriptorSetLayout& descriptorSetLayout,
    DescriptorPool& descriptorPool,
    const char* name)
    : m_descriptors{ descriptors }
{
    m_handle = descriptorPool.Allocate(descriptorSetLayout.GetHandle());
    VkContext::SetDebugName(VK_OBJECT_TYPE_DESCRIPTOR_SET, m_handle, name);
}

void DescriptorSet::Update(const std::vector<Descriptor>& descriptors)
{
    std::vector<VkWriteDescriptorSet> descriptorSetWrites{};
    descriptorSetWrites.reserve(descriptors.size());

    std::vector<VkDescriptorImageInfo> imageInfos;
    std::vector<VkDescriptorBufferInfo> bufferInfos;

    for (const Descriptor& descriptor : descriptors)
    {
        switch (descriptor.GetType())
        {
        case VK_DESCRIPTOR_TYPE_SAMPLER:
        case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
        case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
        case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            imageInfos.emplace_back(VkDescriptorImageInfo{
                .sampler = descriptor.GetSampler(),
                .imageView = descriptor.GetData<Image>()->GetView(),
                .imageLayout = descriptor.GetImageLayout(),
            });
            break;
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
        case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
            bufferInfos.emplace_back(VkDescriptorBufferInfo{
                .buffer = descriptor.GetData<Buffer>()->GetHandle(),
                .offset = 0,
                .range = descriptor.GetRange(),
            });
            break;
        default:
            assert(false && "Descriptor type not supported");
            break;
        }

        VkWriteDescriptorSet writeDescriptorSet{};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.pNext = nullptr;
        writeDescriptorSet.dstSet = m_handle;
        writeDescriptorSet.dstBinding = descriptor.GetBinding();
        writeDescriptorSet.dstArrayElement = 0;
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.descriptorType = descriptor.GetType();
        writeDescriptorSet.pImageInfo = imageInfos.data();
        writeDescriptorSet.pBufferInfo = bufferInfos.data();
        writeDescriptorSet.pTexelBufferView = nullptr;

        descriptorSetWrites.emplace_back(writeDescriptorSet);
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

const std::vector<Descriptor>& DescriptorSet::GetDescriptors() const
{
    return m_descriptors;
}
}
