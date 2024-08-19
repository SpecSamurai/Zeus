#pragma once

#include "core/logger.hpp"

#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

#include <cassert>

#define VKCHECK(expression, msg)                                               \
    do                                                                         \
    {                                                                          \
        VkResult checkVkResult{ expression };                                  \
        if (checkVkResult != VK_SUCCESS)                                       \
        {                                                                      \
            fatal("{} {}", msg, string_VkResult(checkVkResult));               \
            assert(checkVkResult == VK_SUCCESS);                               \
        }                                                                      \
    } while (0)
