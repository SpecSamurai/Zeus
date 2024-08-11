#pragma once

#include "core/logger.hpp"

#include <vulkan/vulkan.h>

#define VKCHECK(expression, msg)                                               \
    {                                                                          \
        VkResult checkVkResult{ expression };                                  \
        if (checkVkResult != VK_SUCCESS)                                       \
        {                                                                      \
            fatal("{} {}", msg, Zeus::vkResultToString(checkVkResult));        \
            assert(checkVkResult == VK_SUCCESS);                               \
        }                                                                      \
    }

namespace Zeus
{
std::uint32_t findMemoryType(
    const VkPhysicalDevice& physicalDevice,
    std::uint32_t typeFilter,
    VkMemoryPropertyFlags properties);

const char* vkResultToString(VkResult result);
};
