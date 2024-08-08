#pragma once

#include <vulkan/vulkan.h>

#include <cstddef>
#include <cstdint>

namespace Zeus
{
VkResult createVkShaderModule(
    const VkDevice& device,
    std::size_t codeSize,
    const std::uint32_t* pCode,
    VkShaderModule& shaderModule);

VkShaderModule loadShader(VkDevice device, const char* filename);
}
