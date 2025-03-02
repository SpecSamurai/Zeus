#pragma once

#include "math/definitions.hpp"

#include <vulkan/vulkan_core.h>

namespace Zeus
{
enum class RendererEntity
{
    MESH,
};

enum class RenderTargets
{
    RENDER_OUTPUT_COLOR,
    RENDER_OUTPUT_DEPTH,
    COUNT
};

enum class ShaderModuleTypes
{
    LINE_VERT,
    FLAT_COLOR_FRAG,
    MESH,
    FRAG_MESH,
    COUNT
};

enum class PipelineTypes
{
    LINES,
    MESH_OPAQUE,
    MESH_TRANSPARENT,
    COUNT
};

// descriptor set 0 for engine-global resources bound once pe frame
struct FrameData
{
    alignas(16) Math::Matrix4x4f view_projection;
};

// descriptor set 1 for per-pass resources, and bound once per pass
// struct Pass
// {
// };

// descriptor set 2 will be used for material resources
// struct Material
// {
// };

// 3 will be used for per-object resources
// struct Object
// {
// };

struct MeshPushConstants
{
    Math::Matrix4x4f modelMatrix;
    VkDeviceAddress vertexBufferAddress;
};
}
