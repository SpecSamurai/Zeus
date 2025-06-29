#pragma once

#include "Material.hpp"
#include "math/definitions.hpp"
#include "math/transformations.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>

namespace Zeus
{
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
    PBR_PASS_VERT,
    PBR_PASS_FRAG,
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

// Bindless engine-global resources bound once per frame
struct MaterialParameters
{
    std::uint32_t flags = 0;
};

struct LightParameters
{
    Math::Vector3f position;
    float intensity;

    Math::Vector3f direction;
    float range;
};

// Constant engine-global resources bound once per frame
struct FrameData
{
    alignas(16) Math::Matrix4x4f view_projection;
    alignas(16) Math::Vector3f viewPosition;
    float specularIntensity;
    alignas(16) Math::Vector4f ambientLight;
    alignas(16) Math::Vector4f directionalLight;
    alignas(16) Math::Vector3f directionalLightPosition;
};

struct PbrPassParameters
{
};

// 128 bytes/resources bound once per draw
struct PassPushConstants
{
    Math::Matrix4x4f transform{ Math::identity<Math::Matrix4x4f>() };
    VkDeviceAddress vertexBufferAddress;
    Material::MaterialIndex materialIndex{ Material::INVALID_MATERIAL_INDEX };
};

static_assert(offsetof(PassPushConstants, transform) == 0);
static_assert(offsetof(PassPushConstants, vertexBufferAddress) == 64);
static_assert(offsetof(PassPushConstants, materialIndex) == 72);
static_assert(sizeof(PassPushConstants) <= 128, "Push constants size limit.");
}
