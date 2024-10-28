#pragma once

#include <vulkan/vulkan.h>

#include <cstddef>
#include <cstdint>

namespace Zeus
{
VkResult createVkShaderModule(
    VkDevice device,
    std::size_t codeSize,
    const std::uint32_t* pCode,
    VkShaderModule* shaderModule);

VkResult loadShader(
    VkShaderModule* shaderModule,
    VkDevice device,
    const char* filename);
}
