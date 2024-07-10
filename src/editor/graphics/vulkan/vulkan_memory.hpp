#pragma once

#include "vulkan_device.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>

namespace Zeus
{
std::uint32_t findMemoryType(
    const std::uint32_t& typeFilter,
    const VkPhysicalDevice& physicalDevice,
    const VkMemoryPropertyFlags& properties);

void CreateBuffer(
    VulkanDevice device,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer& buffer,
    VkDeviceMemory& bufferMemory);

void copyBuffer(
    VulkanDevice device,
    VkCommandPool commandPool,
    VkBuffer srcBuffer,
    VkBuffer dstBuffer,
    VkDeviceSize size);
}
