#pragma once

#include <vulkan/vulkan_core.h>

namespace Zeus
{
class BlendState
{
public:
    BlendState(
        VkBool32 blendEnable = VK_FALSE,
        VkBlendFactor srcColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        VkBlendFactor dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        VkBlendOp colorBlendOp = VK_BLEND_OP_ADD,
        VkBlendFactor srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        VkBlendFactor dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        VkBlendOp alphaBlendOp = VK_BLEND_OP_ADD,
        VkColorComponentFlags colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                               VK_COLOR_COMPONENT_G_BIT |
                                               VK_COLOR_COMPONENT_B_BIT |
                                               VK_COLOR_COMPONENT_A_BIT);

    VkBool32 GetBlendEnable() const;
    VkBlendFactor GetSrcColorBlendFactor() const;
    VkBlendFactor GetDstColorBlendFactor() const;
    VkBlendOp GetColorBlendOp() const;
    VkBlendFactor GetSrcAlphaBlendFactor() const;
    VkBlendFactor GetDstAlphaBlendFactor() const;
    VkBlendOp GetAlphaBlendOp() const;
    VkColorComponentFlags GetColorWriteMask() const;

private:
    VkBool32 m_blendEnable;
    VkBlendFactor m_srcColorBlendFactor;
    VkBlendFactor m_dstColorBlendFactor;
    VkBlendOp m_colorBlendOp;
    VkBlendFactor m_srcAlphaBlendFactor;
    VkBlendFactor m_dstAlphaBlendFactor;
    VkBlendOp m_alphaBlendOp;
    VkColorComponentFlags m_colorWriteMask;
};
}
