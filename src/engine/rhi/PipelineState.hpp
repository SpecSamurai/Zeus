#pragma once

#include "BlendState.hpp"
#include "DepthStencilState.hpp"
#include "RasterizationState.hpp"
#include "Shader.hpp"

#include <vulkan/vulkan_core.h>

#include <vector>

namespace Zeus
{
class PipelineState
{
public:
    PipelineState(
        std::vector<Shader>&& shaders,
        const RasterizationState& rasterizationState,
        const DepthStencilState& depthStencilState,
        const BlendState& blendState,
        VkPrimitiveTopology primitiveTopology,
        const std::vector<VkFormat>& colorAttachmentFormats = {},
        VkFormat depthAttachmentFormat = VK_FORMAT_UNDEFINED,
        VkFormat stencilAttachmentFormat = VK_FORMAT_UNDEFINED);

    bool IsGraphics() const;
    bool IsCompute() const;
    bool HasTessellation() const;

    const std::vector<Shader>& GetShaders() const;
    const RasterizationState& GetRasterizationState() const;
    const DepthStencilState& GetDepthStencilState() const;
    const BlendState& GetBlendState() const;

    const std::vector<VkFormat>& GetColorAttachmentFormats() const;
    VkFormat GetDepthAttachmentFormat() const;
    VkFormat GetStencilAttachmentFormat() const;

    VkPrimitiveTopology GetPrimitiveTopology() const;

private:
    std::vector<Shader> m_shaders;

    RasterizationState m_rasterizationState;
    DepthStencilState m_depthStencilState;
    BlendState m_blendState;

    std::vector<VkFormat> m_colorAttachmentFormats;
    VkFormat m_depthAttachmentFormat;
    VkFormat m_stencilAttachmentFormat;

    // VK_PRIMITIVE_TOPOLOGY_PATCH_LIST
    VkPrimitiveTopology m_primitiveTopology;

    bool m_instancing{ false };
};
}
