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
    // size of the bytecode is specified in bytes, but the bytecode pointer is a
    // uint32_t pointer rather than a char pointer. When you perform a cast like
    // this, you also need to ensure that the data satisfies the alignment
    // requirements of uint32_t. The data is stored in an std::vector where the
    // default allocator already ensures that the data satisfies the worst case
    // alignment requirements.
    VkShaderModuleCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = code.size(),
        .pCode = reinterpret_cast<const uint32_t*>(code.data()),
    };

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
