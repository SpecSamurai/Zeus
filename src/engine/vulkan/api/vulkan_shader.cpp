#include "vulkan_shader.hpp"

#include "logging/logger.hpp"
#include "vulkan_debug.hpp"
#include "vulkan_memory.hpp"

#include <vulkan/vulkan.h>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <vector>

namespace Zeus
{
VkResult createVkShaderModule(
    VkDevice device,
    std::size_t codeSize,
    const std::uint32_t* pCode,
    VkShaderModule* shaderModule)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = codeSize;
    createInfo.pCode = pCode;

    VkResult result{ vkCreateShaderModule(
        device,
        &createInfo,
        allocationCallbacks.get(),
        shaderModule) };

    VKCHECK(result, "Failed to create shader module.");

    return result;
}

VkResult loadShader(
    VkShaderModule* shaderModule,
    VkDevice device,
    const char* filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        LOG_ERROR("Failed to open file: {}", filename);
        assert(file.is_open());

        return VK_ERROR_INITIALIZATION_FAILED;
    }

    std::size_t fileSize{ static_cast<std::size_t>(file.tellg()) };
    assert(fileSize > 0);

    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), static_cast<std::streamsize>(fileSize));

    file.close();

    return createVkShaderModule(
        device,
        fileSize,
        reinterpret_cast<const uint32_t*>(buffer.data()),
        shaderModule);
}
};
