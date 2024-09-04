#include "DescriptorWriter.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <deque>
#include <vector>

namespace Zeus
{
void DescriptorWriter::writeDescriptorImage(
    VkDescriptorType descriptorType,
    std::uint32_t binding,
    VkImageView imageView,
    VkSampler sampler,
    VkImageLayout imageLayout)
{
    VkDescriptorImageInfo& descriptorImageInfo{
        imageInfos.emplace_back(VkDescriptorImageInfo{
            .sampler = sampler,
            .imageView = imageView,
            .imageLayout = imageLayout,
        }),
    };

    VkWriteDescriptorSet writeDescriptorSet{};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = VK_NULL_HANDLE;
    writeDescriptorSet.dstBinding = binding;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.descriptorType = descriptorType;
    writeDescriptorSet.pImageInfo = &descriptorImageInfo;

    descriptorSetWrites.push_back(writeDescriptorSet);
}

void DescriptorWriter::writeDescriptorBuffer(
    VkDescriptorType descriptorType,
    std::uint32_t binding,
    VkBuffer buffer,
    VkDeviceSize size,
    VkDeviceSize offset)
{
    VkDescriptorBufferInfo& descriptorBufferInfo{
        bufferInfos.emplace_back(VkDescriptorBufferInfo{
            .buffer = buffer,
            .offset = offset,
            .range = size,
        }),
    };

    VkWriteDescriptorSet writeDescriptorSet{};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = VK_NULL_HANDLE;
    writeDescriptorSet.dstBinding = binding;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.descriptorType = descriptorType;
    writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;

    descriptorSetWrites.push_back(writeDescriptorSet);
}

void DescriptorWriter::clear()
{
    imageInfos.clear();
    descriptorSetWrites.clear();
    bufferInfos.clear();
}

void DescriptorWriter::updateDescriptorSet(
    VkDevice device,
    VkDescriptorSet descriptorSet)
{
    for (VkWriteDescriptorSet& write : descriptorSetWrites)
    {
        write.dstSet = descriptorSet;
    }

    vkUpdateDescriptorSets(
        device,
        static_cast<std::uint32_t>(descriptorSetWrites.size()),
        descriptorSetWrites.data(),
        0,
        nullptr);
}
}
