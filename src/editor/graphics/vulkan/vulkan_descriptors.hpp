#pragma once

#include <vulkan/vulkan_core.h>

namespace Zeus
{
bool createDescriptorPool(
    const VkDevice& device,
    VkDescriptorPool& descriptorPool);

bool createDescriptorSetLayout(
    const VkDevice& device,
    VkDescriptorSetLayout& descriptorSetLayout);
}
