#pragma once

#include "Definitions.hpp"
#include "VertexInput.hpp"

#include <vulkan/vulkan_core.h>

#include <string>
#include <vector>

namespace Zeus
{
class Shader
{
public:
    Shader(
        const char* filePath,
        VkShaderStageFlagBits shaderStage,
        const char* entryPoint = "main",
        const std::vector<VertexInput>& vertexInputs = {},
        const char* name = nullptr);
    ~Shader();

    void Compile();

    VkShaderModule GetHandle() const;
    bool IsCompiled() const;

    VkShaderStageFlagBits GetShaderStage() const;
    ShaderCompilationState GetCompilationState() const;
    const std::vector<VertexInput>& GetVertexInputs() const;
    const std::string& GetEntryPoint() const;
    const std::string& GetFilePath() const;

private:
    VkShaderModule m_handle{ VK_NULL_HANDLE };
    std::vector<VertexInput> m_vertexInputs;

    std::string m_filePath;
    std::string m_entryPoint;

    VkShaderStageFlagBits m_shaderStage;
    ShaderCompilationState m_compilationState{ ShaderCompilationState::Idle };
};
}
