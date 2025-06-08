#pragma once

#include "math/definitions.hpp"
#include "math/transformations.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>

namespace Zeus
{
namespace Materials
{
using Index = std::uint32_t;

static constexpr Index InvalidIndex{ std::numeric_limits<Index>().max() };
}

namespace Textures
{
using Index = std::uint32_t;

static constexpr Index InvalidIndex{ std::numeric_limits<Index>().max() };
}

enum class RendererEntity : std::uint8_t
{
    MESH_OPAQUE,
    MESH_TRANSPARENT,
    COUNT
};

enum class RenderTarget : std::uint8_t
{
    RENDER_OUTPUT_COLOR,
    RENDER_OUTPUT_DEPTH,
    COUNT
};

enum class ShaderType : std::uint8_t
{
    LINE_VERT,
    FLAT_COLOR_FRAG,
    MESH,
    FRAG_MESH,
    COUNT
};

enum class PipelineType : std::uint8_t
{
    LINES,
    MESH_OPAQUE,
    MESH_TRANSPARENT,
    COUNT
};

enum class SamplerType : std::uint8_t
{
    NEAREST_CLAMP_EDGE,
    LINEAR_CLAMP_EDGE,
    COUNT
};

enum class TextureType : std::uint8_t
{
    // Metalness-Roughness
    BASE_COLOR,
    ROUGHNESS,
    METALNESS,

    // Specular-Glossiness
    DIFFUSE_COLOR, // Albedo
    SPECULAR,
    GLOSSINESS,

    AMBIENT_OCCLUSION,
    NORMAL,
    EMISSION,
    HEIGHT,
    COUNT
};

enum class MaterialProperty : std::uint8_t
{
    ROUGHNESS,
    METALNESS,
    COUNT
};

// descriptor set 0 for engine-global resources bound once pe frame
struct BindlessMaterial
{
};

// descriptor set 1 for engine-global resources bound once pe frame
struct UniformBufferFrame
{
    alignas(16) Math::Matrix4x4f view_projection;
};

// 128 bytes/Resource per draw
struct PassPushConstants
{
    Math::Matrix4x4f model{ Math::identity<Math::Matrix4x4f>() };
    alignas(8) VkDeviceAddress vertexBufferAddress;
    alignas(4) Materials::Index materialIndex{ Materials::InvalidIndex };
};

static_assert(sizeof(PassPushConstants) <= 128, "Push constants size limit.");

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

struct GPUSceneData
{
    Math::Vector4f fogColor;     // w is for exponent
    Math::Vector4f fogDistances; // x for min, y for max, zw unused.
    Math::Vector4f ambientColor;
    Math::Vector4f sunlightDirection; // w for sun power
    Math::Vector4f sunlightColor;
};
}
