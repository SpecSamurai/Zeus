#pragma once

#include "math/definitions.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>

namespace Zeus
{
enum class RendererEntity : std::uint8_t
{
    MESH_OPAQUE,
    MESH_TRANSPARENT,
};

enum class RenderTargets : std::uint8_t
{
    RENDER_OUTPUT_COLOR,
    RENDER_OUTPUT_DEPTH,
    COUNT
};

enum class ShaderModuleTypes : std::uint8_t
{
    LINE_VERT,
    FLAT_COLOR_FRAG,
    MESH,
    FRAG_MESH,
    COUNT
};

enum class PipelineTypes : std::uint8_t
{
    LINES,
    MESH_OPAQUE,
    MESH_TRANSPARENT,
    COUNT
};

enum class TextureTypes : std::uint8_t
{
    COLOR,
    ROUGHNESS,
    METALNESS,
    AMBIENT_OCCLUSION,
    NORMAL,
    EMISSION,
    HEIGHT,
    COUNT
};

enum class MaterialProperties : std::uint8_t
{
    ROUGHNESS,
    METALNESS,
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
    Math::Matrix4x4f model;
    VkDeviceAddress vertexBufferAddress;
};
}
