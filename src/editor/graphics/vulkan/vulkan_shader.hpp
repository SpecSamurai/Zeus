#pragma once

#include <vulkan/vulkan_core.h>

#include <vector>

namespace Zeus
{
bool createVkShaderModule(
    const VkDevice& device,
    const std::vector<char>& code,
    VkShaderModule& shaderModule);
}
