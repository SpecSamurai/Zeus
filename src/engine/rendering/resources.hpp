#pragma once

#include "vulkan/BlendState.hpp"
#include "vulkan/DepthStencilState.hpp"
#include "vulkan/Image.hpp"
#include "vulkan/RasterizationState.hpp"
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

namespace Rasterization
{
inline const RasterizationState Default{ RasterizationState() };
}

namespace DepthStencil
{
inline const DepthStencilState DefaultDisabled{ DepthStencilState() };
inline const DepthStencilState DefaultEnabled{ DepthStencilState(
    VK_TRUE,
    VK_TRUE,
    VK_COMPARE_OP_GREATER_OR_EQUAL,
    VK_FALSE,
    VK_FALSE) };
}

// out = srcColor * srcColorBlendFactor <op> dstColor * dstColorBlendFactor
namespace Blending
{
inline const BlendState Disabled{ BlendState() };

// out = srcColor.rgb * srcColor.a + dstColor.rgb * 1.0
inline const BlendState AdditiveBlending{ BlendState(
    VK_TRUE,
    VK_BLEND_FACTOR_SRC_ALPHA,
    VK_BLEND_FACTOR_ONE,
    VK_BLEND_OP_ADD,
    VK_BLEND_FACTOR_ONE,
    VK_BLEND_FACTOR_ZERO,
    VK_BLEND_OP_ADD) };

// out = srcColor.rgb * srcColor.a + dstColor.rgb * (1.0 - srcColor.a)
inline const BlendState AlphaBlending{ BlendState(
    VK_TRUE,
    VK_BLEND_FACTOR_SRC_ALPHA,
    VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    VK_BLEND_OP_ADD,
    VK_BLEND_FACTOR_ONE,
    VK_BLEND_FACTOR_ZERO,
    VK_BLEND_OP_ADD) };
}
}
