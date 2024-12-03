#pragma once

#include <fastgltf/types.hpp>
#include <vulkan/vulkan_core.h>

namespace Zeus
{
VkFilter toVkFilter(fastgltf::Filter filter);
VkSamplerMipmapMode toVkSamplerMipmapMode(fastgltf::Filter filter);
VkSamplerAddressMode toVkSamplerAddressMode(fastgltf::Wrap wrap);
}
