#include "utility.hpp"

#include "math/Quaternion.hpp"
#include "math/definitions.hpp"

#include <fastgltf/types.hpp>
#include <vulkan/vulkan_core.h>

#include <cassert>

namespace Zeus
{
VkFilter toVkFilter(fastgltf::Filter filter)
{
    switch (filter)
    {
    case fastgltf::Filter::Nearest:
    case fastgltf::Filter::NearestMipMapNearest:
    case fastgltf::Filter::NearestMipMapLinear:
        return VK_FILTER_NEAREST;

    case fastgltf::Filter::Linear:
    case fastgltf::Filter::LinearMipMapNearest:
    case fastgltf::Filter::LinearMipMapLinear:
    default:
        return VK_FILTER_LINEAR;
    }
}

VkSamplerMipmapMode toVkSamplerMipmapMode(fastgltf::Filter filter)
{
    switch (filter)
    {
    case fastgltf::Filter::NearestMipMapNearest:
    case fastgltf::Filter::LinearMipMapNearest:
        return VK_SAMPLER_MIPMAP_MODE_NEAREST;

    case fastgltf::Filter::NearestMipMapLinear:
    case fastgltf::Filter::LinearMipMapLinear:
    default:
        return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    }
}

VkSamplerAddressMode toVkSamplerAddressMode(fastgltf::Wrap wrap)
{
    switch (wrap)
    {
    case fastgltf::Wrap::ClampToEdge:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case fastgltf::Wrap::MirroredRepeat:
        return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    case fastgltf::Wrap::Repeat:
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    default:
        assert(false && "Unsupported type");
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    }
}

Vector2f toVector2f(fastgltf::math::fvec2 vec2)
{
    return Vector2f(vec2.x(), vec2.y());
}

Vector3f toVector3f(fastgltf::math::fvec3 vec3)
{
    return Vector3f(vec3.x(), vec3.y(), vec3.z());
}

Vector4f toVector4f(fastgltf::math::fvec4 vec4)
{
    return Vector4f(vec4.x(), vec4.y(), vec4.z(), vec4.w());
}

Quaternion<float> toQuaternion(fastgltf::math::fquat quat)
{
    return Quaternion(quat.x(), quat.y(), quat.z(), quat.w());
}
}
