#include "BlendState.hpp"

#include <vulkan/vulkan_core.h>

namespace Zeus
{
const BlendState BlendState::Disabled{ BlendState() };

const BlendState BlendState::AdditiveBlending{ BlendState(
    VK_TRUE,
    VK_BLEND_FACTOR_SRC_ALPHA,
    VK_BLEND_FACTOR_ONE,
    VK_BLEND_OP_ADD,
    VK_BLEND_FACTOR_ONE,
    VK_BLEND_FACTOR_ZERO,
    VK_BLEND_OP_ADD) };

const BlendState BlendState::AlphaBlending{ BlendState(
    VK_TRUE,
    VK_BLEND_FACTOR_SRC_ALPHA,
    VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    VK_BLEND_OP_ADD,
    VK_BLEND_FACTOR_ONE,
    VK_BLEND_FACTOR_ZERO,
    VK_BLEND_OP_ADD) };

BlendState::BlendState(
    VkBool32 blendEnable,
    VkBlendFactor srcColorBlendFactor,
    VkBlendFactor dstColorBlendFactor,
    VkBlendOp colorBlendOp,
    VkBlendFactor srcAlphaBlendFactor,
    VkBlendFactor dstAlphaBlendFactor,
    VkBlendOp alphaBlendOp,
    VkColorComponentFlags colorWriteMask)
    : m_blendEnable{ blendEnable },
      m_srcColorBlendFactor{ srcColorBlendFactor },
      m_dstColorBlendFactor{ dstColorBlendFactor },
      m_colorBlendOp{ colorBlendOp },
      m_srcAlphaBlendFactor{ srcAlphaBlendFactor },
      m_dstAlphaBlendFactor{ dstAlphaBlendFactor },
      m_alphaBlendOp{ alphaBlendOp },
      m_colorWriteMask{ colorWriteMask }
{
}

VkBool32 BlendState::GetBlendEnable() const
{
    return m_blendEnable;
}

VkBlendFactor BlendState::GetSrcColorBlendFactor() const
{
    return m_srcColorBlendFactor;
}

VkBlendFactor BlendState::GetDstColorBlendFactor() const
{
    return m_dstColorBlendFactor;
}

VkBlendOp BlendState::GetColorBlendOp() const
{
    return m_colorBlendOp;
}

VkBlendFactor BlendState::GetSrcAlphaBlendFactor() const
{
    return m_srcAlphaBlendFactor;
}

VkBlendFactor BlendState::GetDstAlphaBlendFactor() const
{
    return m_dstAlphaBlendFactor;
}

VkBlendOp BlendState::GetAlphaBlendOp() const
{
    return m_alphaBlendOp;
}

VkColorComponentFlags BlendState::GetColorWriteMask() const
{
    return m_colorWriteMask;
}
}
