#pragma once

#include "Device.hpp"

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

bool beginVkCommandBuffer(VkCommandBuffer& commandBuffer);
bool endVkCommandBuffer(VkCommandBuffer& commandBuffer);

bool beginOneTimeVkCommandBuffer(
    const VkDevice& device,
    const VkCommandPool& commandPool,
    VkCommandBuffer& commandBuffer);

bool endOneTimeVkCommandBuffer(
    const Device& device,
    const VkCommandPool& commandPool,
    VkCommandBuffer& commandBuffer);

void cmdBeginVkRenderPass(
    const VkRenderPass& renderPass,
    const VkExtent2D& extent,
    const VkFramebuffer& framebuffer,
    const std::array<VkClearValue, 2>& clearValues,
    VkCommandBuffer& commandBuffer);

void cmdSetVkViewport(const VkExtent2D& extent, VkCommandBuffer& commandBuffer);
void cmdSetVkScissor(const VkExtent2D& extent, VkCommandBuffer& commandBuffer);
}
