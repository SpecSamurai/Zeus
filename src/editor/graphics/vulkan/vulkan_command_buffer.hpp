#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
bool createVkCommandPool(
    const VkDevice& device,
    std::uint32_t queueFamilyIndex,
    VkCommandPool& commandPool);

bool allocateVkCommandBuffers(
    const VkDevice& device,
    const VkCommandPool& commandPool,
    std::vector<VkCommandBuffer>& commandBuffers);
}
