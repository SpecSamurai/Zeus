#pragma once

#include <cstdint>

namespace Zeus
{
enum class ResourceType : std::uint8_t
{
    CommandPool,
    DescriptorPool,
    DescriptorSetLayout,
    Fence,
    Pipeline,
    PipelineLayout,
    Semaphore,
    Shader,
    Sampler,
    Count,
};

enum class QueueType : std::uint8_t
{
    Graphics,
    Present,
    Transfer,
    Compute,
};

enum class ShaderCompilationState : std::uint8_t
{
    Idle,
    Compiling,
    Compiled,
    Failed
};

enum class ImageType : std::uint8_t
{
    Cube,
    Texture2D,
    Texture2DArray,
    Texture3D,
};

constexpr const char* resourceTypeToString(ResourceType type)
{
    switch (type)
    {
    case ResourceType::CommandPool:
        return "CommandPool";
    case ResourceType::DescriptorPool:
        return "DescriptorPool";
    case ResourceType::DescriptorSetLayout:
        return "DescriptorSetLayout";
    case ResourceType::Fence:
        return "Fence";
    case ResourceType::Pipeline:
        return "Pipeline";
    case ResourceType::PipelineLayout:
        return "PipelineLayout";
    case ResourceType::Semaphore:
        return "Semaphore";
    case ResourceType::Shader:
        return "Shader";
    case ResourceType::Sampler:
        return "Sampler";
    default:
        return "Unknown";
    }
}
}
