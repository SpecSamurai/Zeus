#pragma once

#include <fastgltf/core.hpp>
#include <vulkan/vulkan_core.h>

namespace Zeus
{
VkFilter toVkFilter(fastgltf::Filter filter);
VkSamplerMipmapMode toVkSamplerMipmapMode(fastgltf::Filter filter);
}
