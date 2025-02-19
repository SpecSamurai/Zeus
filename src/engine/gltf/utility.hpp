#pragma once

#include "math/Quaternion.hpp"
#include "math/definitions.hpp"

#include <fastgltf/types.hpp>
#include <vulkan/vulkan_core.h>

namespace Zeus
{
VkFilter toVkFilter(fastgltf::Filter filter);
VkSamplerMipmapMode toVkSamplerMipmapMode(fastgltf::Filter filter);
VkSamplerAddressMode toVkSamplerAddressMode(fastgltf::Wrap wrap);

Math::Vector2f toVector2f(fastgltf::math::fvec2 vec2);
Math::Vector3f toVector3f(fastgltf::math::fvec3 vec3);
Math::Vector4f toVector4f(fastgltf::math::fvec4 vec4);
Math::Quaternion<float> toQuaternion(fastgltf::math::fquat quat);
}
