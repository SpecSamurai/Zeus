#pragma once

#include <cstdint>

namespace Zeus
{
enum class ResourceType : std::uint8_t
{
    Buffer,
    CommandBuffer,
    CommandPool,
    DescriptorSet,
    DescriptorSetLayout,
    Fence,
    Image,
    ImageView,
    Pipeline,
    PipelineLayout,
    Queue,
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
}
