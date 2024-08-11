#include "MemoryAllocator.hpp"

#include "core/logger.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>

namespace Zeus
{
std::uint32_t findMemoryType(
    const VkPhysicalDevice& physicalDevice,
    std::uint32_t typeFilter,
    VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    for (std::uint32_t i{ 0 }; i < memoryProperties.memoryTypeCount; ++i)
    {
        if ((typeFilter & (1 << i)) &&
            (memoryProperties.memoryTypes[i].propertyFlags & properties) ==
                properties)
        {
            return i;
        }
    }

    error("Failed to find memory type");
    return 0;
}
}
