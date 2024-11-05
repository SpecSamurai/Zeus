#include "DepthStencilState.hpp"

#include <vulkan/vulkan_core.h>

namespace Zeus
{
const DepthStencilState DepthStencilState::Disabled{ DepthStencilState() };
const DepthStencilState DepthStencilState::Enabled{ DepthStencilState(
    VK_TRUE,
    VK_TRUE,
    VK_COMPARE_OP_GREATER_OR_EQUAL,
    VK_FALSE,
    VK_FALSE) };

DepthStencilState::DepthStencilState(
    VkBool32 depthTestEnable,
    VkBool32 depthWriteEnable,
    VkCompareOp depthCompareOp,
    VkBool32 depthBoundsTestEnable,
    VkBool32 stencilTestEnable)
    : m_depthTestEnable{ depthTestEnable },
      m_depthWriteEnable{ depthWriteEnable },
      m_depthCompareOp{ depthCompareOp },
      m_depthBoundsTestEnable{ depthBoundsTestEnable },
      m_stencilTestEnable{ stencilTestEnable }
{
}

VkBool32 DepthStencilState::GetDepthTestEnable() const
{
    return m_depthTestEnable;
}

VkBool32 DepthStencilState::GetDepthWriteEnable() const
{
    return m_depthWriteEnable;
}

VkCompareOp DepthStencilState::GetDepthCompareOp() const
{
    return m_depthCompareOp;
}

VkBool32 DepthStencilState::GetDepthBoundsTestEnable() const
{
    return m_depthBoundsTestEnable;
}

VkBool32 DepthStencilState::GetStencilTestEnable() const
{
    return m_stencilTestEnable;
}
}
