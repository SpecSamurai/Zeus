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
    Image,
    ImageView,
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
}
