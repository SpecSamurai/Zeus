#pragma once

#include <vulkan/vulkan_core.h>

namespace Zeus
{
class RasterizationState
{
public:
    RasterizationState(
        VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL,
        VkCullModeFlags cullMode = VK_CULL_MODE_NONE,
        VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE,
        VkBool32 depthClampEnable = VK_FALSE,
        VkBool32 depthBiasEnable = VK_FALSE,
        float depthBiasConstantFactor = 0.f,
        float depthBiasClamp = 0.f,
        float depthBiasSlopeFactor = 0.f,
        float lineWidth = 1.f);

    VkPolygonMode GetPolygonMode() const;
    VkCullModeFlags GetCullMode() const;
    VkFrontFace GetFrontFace() const;

    float GetDepthBiasConstantFactor() const;
    float GetDepthBiasClamp() const;
    float GetDepthBiasSlopeFactor() const;
    float GetLineWidth() const;

    VkBool32 GetDepthClampEnable() const;
    VkBool32 GetDepthBiasEnable() const;

private:
    VkPolygonMode m_polygonMode;
    VkCullModeFlags m_cullMode;
    VkFrontFace m_frontFace;

    float m_depthBiasConstantFactor;
    float m_depthBiasClamp;
    float m_depthBiasSlopeFactor;
    float m_lineWidth;

    VkBool32 m_depthClampEnable;
    VkBool32 m_depthBiasEnable;
};
}
