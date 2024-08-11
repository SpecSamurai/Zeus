#pragma once

#include "core/logger.hpp"

#include <vulkan/vulkan.h>

#include <cassert>

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
const char* vkResultToString(VkResult result);
};
