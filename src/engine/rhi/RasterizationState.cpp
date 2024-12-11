#include "RasterizationState.hpp"

namespace Zeus
{
RasterizationState::RasterizationState(
    VkPolygonMode polygonMode,
    VkCullModeFlags cullMode,
    VkFrontFace frontFace,
    VkBool32 depthClampEnable,
    VkBool32 depthBiasEnable,
    float depthBiasConstantFactor,
    float depthBiasClamp,
    float depthBiasSlopeFactor,
    float lineWidth)
    : m_polygonMode{ polygonMode },
      m_cullMode{ cullMode },
      m_frontFace{ frontFace },
      m_depthBiasConstantFactor{ depthBiasConstantFactor },
      m_depthBiasClamp{ depthBiasClamp },
      m_depthBiasSlopeFactor{ depthBiasSlopeFactor },
      m_lineWidth{ lineWidth },
      m_depthClampEnable{ depthClampEnable },
      m_depthBiasEnable{ depthBiasEnable }
{
}

VkPolygonMode RasterizationState::GetPolygonMode() const
{
    return m_polygonMode;
}

VkCullModeFlags RasterizationState::GetCullMode() const
{
    return m_cullMode;
}

VkFrontFace RasterizationState::GetFrontFace() const
{
    return m_frontFace;
}

float RasterizationState::GetDepthBiasConstantFactor() const
{
    return m_depthBiasConstantFactor;
}

float RasterizationState::GetDepthBiasClamp() const
{
    return m_depthBiasClamp;
}

float RasterizationState::GetDepthBiasSlopeFactor() const
{
    return m_depthBiasSlopeFactor;
}

float RasterizationState::GetLineWidth() const
{
    return m_lineWidth;
}

VkBool32 RasterizationState::GetDepthClampEnable() const
{
    return m_depthClampEnable;
}

VkBool32 RasterizationState::GetDepthBiasEnable() const
{
    return m_depthBiasEnable;
}
}
