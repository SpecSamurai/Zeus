#pragma once

#include "BlendState.hpp"
#include "DepthStencilState.hpp"
#include "DescriptorSetLayout.hpp"
#include "RasterizationState.hpp"
#include "Shader.hpp"

#include <vulkan/vulkan_core.h>

#include <string_view>
#include <vector>

namespace Zeus
{
class Pipeline
{
public:
    Pipeline() = default;
    Pipeline(
        std::string_view name,
        const RasterizationState& rasterizationState,
        const DepthStencilState& depthStencilState,
        const BlendState& blendState,
        VkPrimitiveTopology primitiveTopology,
        std::vector<const Shader*> shaders,
        const std::vector<const DescriptorSetLayout*>& descriptorSetLayouts,
        const std::vector<VkPushConstantRange>& pushConstants,
        const std::vector<VkFormat>& colorAttachmentFormats = {},
        VkFormat depthAttachmentFormat = VK_FORMAT_UNDEFINED,
        VkFormat stencilAttachmentFormat = VK_FORMAT_UNDEFINED);

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    Pipeline(Pipeline&& other) noexcept;
    Pipeline& operator=(Pipeline&& other);

    ~Pipeline();

    void Destroy();

    VkPipeline GetHandle() const;
    VkPipelineLayout GetLayout() const;
    std::string_view GetName() const;
    VkPipelineBindPoint GetBindPoint() const;
    const std::vector<const Shader*>& GetShaders() const;

    bool IsGraphics() const;
    bool IsCompute() const;
    bool HasTessellation() const;

private:
    void CreateGraphicsPipeline();
    void CreateComputePipeline();
    VkPipelineShaderStageCreateInfo CreatePipelineShaderStageInfo(
        VkShaderStageFlagBits stage,
        VkShaderModule module,
        const char* pName) const;

private:
    std::string_view m_name{};

    VkPipeline m_handle{ VK_NULL_HANDLE };
    VkPipelineLayout m_pipelineLayout{ VK_NULL_HANDLE };
    std::vector<const Shader*> m_shaders;
    VkPipelineBindPoint m_bindPoint;

    RasterizationState m_rasterizationState;
    DepthStencilState m_depthStencilState;
    BlendState m_blendState;

    std::vector<VkFormat> m_colorAttachmentFormats;
    VkFormat m_depthAttachmentFormat;
    VkFormat m_stencilAttachmentFormat;

    VkPrimitiveTopology m_primitiveTopology;

    bool m_instancing{ false };
};
}
