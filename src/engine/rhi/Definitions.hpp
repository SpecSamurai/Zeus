#pragma once

#include <cstdint>

namespace Zeus
{
enum class ResourceType : std::uint8_t
{
    Buffer,
    CommandPool,
    DescriptorPool,
    DescriptorSetLayout,
    Fence,
    ImageView,
    Image,
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
    Texture3D,
};
}
