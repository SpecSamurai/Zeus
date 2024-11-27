#pragma once

#include "vulkan/Image.hpp"
#include "vulkan/Sampler.hpp"

namespace Zeus
{
void InitializeDefaultResources();
void DestroyDefaultResources();

namespace Textures
{
void InitializeTextures();

inline Image DebugCheckerboard;
}

namespace Samplers
{
void InitializeSamplers();

inline Sampler DefaultLinear;
inline Sampler DefaultNearest;
}
}
