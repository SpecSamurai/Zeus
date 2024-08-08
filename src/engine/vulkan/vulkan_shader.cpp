#include "vulkan_shader.hpp"

#include "vulkan/vulkan_utils.hpp"

#include "core/logger.hpp"

#include <vulkan/vulkan.h>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <vector>

namespace Zeus
{
VkResult createVkShaderModule(
    const VkDevice& device,
    std::size_t codeSize,
    const std::uint32_t* pCode,
    VkShaderModule& shaderModule)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = codeSize;
    createInfo.pCode = pCode;

    VkResult result{
        vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule)
    };

    CHECK_VKRESULT(result, "Failed to create shader module.");

    return result;
}

VkShaderModule loadShader(VkDevice device, const char* filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (!file.is_open())
    {
        error("Failed to open file: {}", filename);
        assert(file.is_open());
    }

    std::size_t fileSize{ static_cast<std::size_t>(file.tellg()) };
    assert(fileSize > 0);

    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), static_cast<std::streamsize>(fileSize));

    file.close();

    VkShaderModule shaderModule;
    createVkShaderModule(
        device,
        fileSize,
        reinterpret_cast<const uint32_t*>(buffer.data()),
        shaderModule);

    return shaderModule;
}
};
