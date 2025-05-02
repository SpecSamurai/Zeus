#pragma once

#include "Definitions.hpp"
#include "Descriptor.hpp"
#include "VertexInput.hpp"

#include <vulkan/vulkan_core.h>

#include <string_view>
#include <vector>

namespace Zeus
{
class Shader
{
public:
    Shader() = default;
    Shader(
        std::string_view name,
        const char* filePath,
        VkShaderStageFlagBits shaderStage,
        const char* entryPoint = "main",
        const std::vector<VertexInput>&& vertexInputs = {});

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
    const std::vector<Descriptor>& GetDescriptors() const;
    const char* GetEntryPoint() const;
    const char* GetFilePath() const;

    constexpr std::string_view GetName() const noexcept
    {
        return m_name;
    }

private:
    VkResult loadShader();

private:
    VkShaderModule m_handle{ VK_NULL_HANDLE };
    std::vector<VertexInput> m_vertexInputs;
    std::vector<Descriptor> m_descriptors;

    const char* m_entryPoint;
    const char* m_filePath;

    VkShaderStageFlagBits m_shaderStage;
    ShaderCompilationState m_compilationState{ ShaderCompilationState::Idle };

    std::string_view m_name;
};
}
