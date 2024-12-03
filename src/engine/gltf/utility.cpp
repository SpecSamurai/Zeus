#include "utility.hpp"

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
}
