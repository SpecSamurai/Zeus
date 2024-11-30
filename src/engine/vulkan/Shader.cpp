#include "Shader.hpp"

#include "Definitions.hpp"
#include "VkContext.hpp"
#include "rhi/vulkan_debug.hpp"

#include <string_view>
#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <vector>

namespace Zeus
{
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

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = fileSize;
    createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

    VkResult result{ vkCreateShaderModule(
        device,
        &createInfo,
        allocationCallbacks.get(),
        shaderModule) };

    VKCHECK(result, "Failed to create shader module.");

    return result;
}

Shader::Shader(
    const char* filePath,
    VkShaderStageFlagBits shaderStage,
    const char* entryPoint,
    const std::vector<VertexInput>&& vertexInputs,
    std::string_view name)
    : m_vertexInputs{ std::move(vertexInputs) },
      m_entryPoint(entryPoint),
      m_filePath(filePath),
      m_shaderStage{ shaderStage }
{
    VkResult result{
        loadShader(&m_handle, VkContext::GetLogicalDevice(), m_filePath)
    };

    m_compilationState = result == VK_SUCCESS ? ShaderCompilationState::Compiled
                                              : ShaderCompilationState::Failed;

    VkContext::SetDebugName(VK_OBJECT_TYPE_SHADER_MODULE, m_handle, name);
}

Shader::Shader(Shader&& other) noexcept
    : m_handle{ other.m_handle },
      m_vertexInputs{ std::move(other.m_vertexInputs) },
      m_entryPoint{ other.m_entryPoint },
      m_filePath{ other.m_filePath },
      m_shaderStage{ other.m_shaderStage },
      m_compilationState{ other.m_compilationState }
{
    other.m_handle = VK_NULL_HANDLE;
}

Shader& Shader::operator=(Shader&& other)
{
    if (this != &other)
    {
        if (m_handle != VK_NULL_HANDLE)
        {
            Destroy();
        }

        m_handle = other.m_handle;
        m_vertexInputs = other.m_vertexInputs;
        m_filePath = other.m_filePath;
        m_entryPoint = other.m_entryPoint;
        m_shaderStage = other.m_shaderStage;
        m_compilationState = other.m_compilationState;

        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}

Shader::~Shader()
{
    if (m_handle == VK_NULL_HANDLE)
        return;

    VkContext::GetDeletionQueue().Add(ResourceType::Shader, m_handle);
    m_handle = VK_NULL_HANDLE;
}

void Shader::Destroy()
{
    vkDestroyShaderModule(
        VkContext::GetLogicalDevice(),
        m_handle,
        allocationCallbacks.get());
    m_handle = VK_NULL_HANDLE;
}

void Shader::Compile()
{
}

VkShaderModule Shader::GetHandle() const
{
    return m_handle;
}

bool Shader::IsCompiled() const
{
    return m_compilationState == ShaderCompilationState::Compiled;
}

VkShaderStageFlagBits Shader::GetShaderStage() const
{
    return m_shaderStage;
}

ShaderCompilationState Shader::GetCompilationState() const
{
    return m_compilationState;
}

const std::vector<VertexInput>& Shader::GetVertexInputs() const
{
    assert(m_shaderStage == VK_SHADER_STAGE_VERTEX_BIT);
    return m_vertexInputs;
}

const char* Shader::GetEntryPoint() const
{
    return m_entryPoint;
}

const char* Shader::GetFilePath() const
{
    return m_filePath;
}
}
