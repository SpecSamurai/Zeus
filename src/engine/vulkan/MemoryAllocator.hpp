#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>

namespace Zeus
{
std::uint32_t findMemoryType(
    const VkPhysicalDevice& physicalDevice,
    std::uint32_t typeFilter,
    VkMemoryPropertyFlags properties);

class MemoryAllocator
{
};
}
