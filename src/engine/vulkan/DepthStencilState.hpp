#pragma once

#include <vulkan/vulkan_core.h>

namespace Zeus
{
class DepthStencilState
{
public:
    DepthStencilState(
        VkBool32 depthTestEnable = VK_FALSE,
        VkBool32 depthWriteEnable = VK_FALSE,
        VkCompareOp depthCompareOp = VK_COMPARE_OP_NEVER,
        VkBool32 depthBoundsTestEnable = VK_FALSE,
        VkBool32 stencilTestEnable = VK_FALSE);

    VkBool32 GetDepthTestEnable() const;
    VkBool32 GetDepthWriteEnable() const;
    VkCompareOp GetDepthCompareOp() const;
    VkBool32 GetDepthBoundsTestEnable() const;
    VkBool32 GetStencilTestEnable() const;

    static const DepthStencilState Disabled;
    static const DepthStencilState Enabled;

private:
    VkBool32 m_depthTestEnable;
    VkBool32 m_depthWriteEnable;
    VkCompareOp m_depthCompareOp;
    VkBool32 m_depthBoundsTestEnable;
    VkBool32 m_stencilTestEnable;
};
}
