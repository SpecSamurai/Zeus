#pragma once

#include "Definitions.hpp"
#include "VertexInput.hpp"

#include <vulkan/vulkan_core.h>

#include <vector>

namespace Zeus
{
class Shader
{
public:
    Shader() = default;
    Shader(
        const char* filePath,
        VkShaderStageFlagBits shaderStage,
        const char* entryPoint = "main",
        const std::vector<VertexInput>&& vertexInputs = {},
        const char* name = nullptr);

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other);

    ~Shader();

    void Destroy();

    void Compile();

    VkShaderModule GetHandle() const;
    bool IsCompiled() const;

    VkShaderStageFlagBits GetShaderStage() const;
    ShaderCompilationState GetCompilationState() const;
    const std::vector<VertexInput>& GetVertexInputs() const;
    const char* GetEntryPoint() const;
    const char* GetFilePath() const;

private:
    VkShaderModule m_handle{ VK_NULL_HANDLE };
    std::vector<VertexInput> m_vertexInputs;

    const char* m_entryPoint;
    const char* m_filePath;

    VkShaderStageFlagBits m_shaderStage;
    ShaderCompilationState m_compilationState{ ShaderCompilationState::Idle };
};
}
