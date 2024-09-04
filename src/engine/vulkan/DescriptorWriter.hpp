#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <deque>
#include <vector>

namespace Zeus
{
class DescriptorWriter
{
public:
    void writeDescriptorImage(
        VkDescriptorType descriptorType,
        std::uint32_t binding,
        VkImageView imageView,
        VkSampler sampler,
        VkImageLayout imageLayout);

    void writeDescriptorBuffer(
        VkDescriptorType descriptorType,
        std::uint32_t binding,
        VkBuffer buffer,
        VkDeviceSize size,
        VkDeviceSize offset);

    void clear();

    void updateDescriptorSet(VkDevice device, VkDescriptorSet descriptorSet);

private:
    std::deque<VkDescriptorImageInfo> imageInfos;
    std::deque<VkDescriptorBufferInfo> bufferInfos;
    std::vector<VkWriteDescriptorSet> descriptorSetWrites;
};
}
