#include "vulkan_shader.hpp"

#include "vulkan_utils.hpp"

#include <core/logger.hpp>

#include <vulkan/vulkan_core.h>

namespace Zeus
{
bool createVkShaderModule(
    const VkDevice& device,
    const std::vector<char>& code,
    VkShaderModule& shaderModule)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkResult result{
        vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule)
    };

    if (result != VK_SUCCESS)
    {
        error("Failed to create shader module. {}", vkResultToString(result));
    }

    return result == VK_SUCCESS;
}
};
