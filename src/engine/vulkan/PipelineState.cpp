#include "PipelineState.hpp"

namespace Zeus
{
PipelineState::PipelineState(
    const std::vector<Shader>& shaders,
    const RasterizationState& rasterizationState,
    const DepthStencilState& depthStencilState,
    const BlendState& blendState,
    VkPrimitiveTopology primitiveTopology,
    const std::vector<VkFormat>& colorAttachmentFormats,
    VkFormat depthAttachmentFormat,
    VkFormat stencilAttachmentFormat)
    : m_shaders{ shaders },
      m_rasterizationState{ rasterizationState },
      m_depthStencilState{ depthStencilState },
      m_blendState{ blendState },
      m_colorAttachmentFormats{ colorAttachmentFormats },
      m_depthAttachmentFormat{ depthAttachmentFormat },
      m_stencilAttachmentFormat{ stencilAttachmentFormat },
      m_primitiveTopology{ primitiveTopology }
{
}

bool PipelineState::IsGraphics() const
{
    return m_shaders.size() >= 2;
}

bool PipelineState::IsCompute() const
{
    return m_shaders.size() == 1 &&
           m_shaders.front().GetShaderStage() == VK_SHADER_STAGE_COMPUTE_BIT;
}

bool PipelineState::HasTessellation() const
{
    return false;
}

const std::vector<Shader>& PipelineState::GetShaders() const
{
    return m_shaders;
}

const RasterizationState& PipelineState::GetRasterizationState() const
{
    return m_rasterizationState;
}

const DepthStencilState& PipelineState::GetDepthStencilState() const
{
    return m_depthStencilState;
}

const BlendState& PipelineState::GetBlendState() const
{
    return m_blendState;
}

const std::vector<VkFormat>& PipelineState::GetColorAttachmentFormats() const
{
    return m_colorAttachmentFormats;
}

VkFormat PipelineState::GetDepthAttachmentFormat() const
{
    return m_depthAttachmentFormat;
}

VkFormat PipelineState::GetStencilAttachmentFormat() const
{
    return m_stencilAttachmentFormat;
}

VkPrimitiveTopology PipelineState::GetPrimitiveTopology() const
{
    return m_primitiveTopology;
}
}
