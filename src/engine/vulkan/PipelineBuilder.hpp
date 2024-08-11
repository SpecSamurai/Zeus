#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
class PipelineBuilder
{
public:
    VkPipeline build(VkDevice device, VkPipelineLayout pipelineLayout);

    PipelineBuilder& addShaderStage(
        VkShaderStageFlagBits stage,
        VkShaderModule module);

    PipelineBuilder& addInputBindingDescription(
        std::uint32_t binding,
        std::uint32_t stride,
        VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX);

    PipelineBuilder& addInputAttributeDescription(
        std::uint32_t location,
        std::uint32_t binding,
        VkFormat format,
        std::uint32_t offset);

    PipelineBuilder& setDynamicStates(
        std::vector<VkDynamicState> dynamicStates);

    PipelineBuilder& setRenderPass(
        VkRenderPass renderPass,
        std::uint32_t subpass = 0);

    PipelineBuilder& setPolygonMode(VkPolygonMode polygonMode);

    PipelineBuilder& setCullMode(
        VkCullModeFlags cullMode,
        VkFrontFace frontFace);

    PipelineBuilder& clear();

private:
    struct PipelineCreateInfo
    {
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};
        std::vector<VkVertexInputBindingDescription>
            vertexInputBindingDescription{};
        std::vector<VkVertexInputAttributeDescription>
            vertexInputAttributeDescription{};

        // const VkPipelineShaderStageCreateInfo* pStages;
        // const VkPipelineVertexInputStateCreateInfo* pVertexInputState;
        // const VkPipelineInputAssemblyStateCreateInfo* pInputAssemblyState;
        // const VkPipelineTessellationStateCreateInfo* pTessellationState;
        // const VkPipelineMultisampleStateCreateInfo* pMultisampleState;
        // const VkPipelineDepthStencilStateCreateInfo* pDepthStencilState;
        // const VkPipelineColorBlendStateCreateInfo* pColorBlendState;
        // const VkPipelineDynamicStateCreateInfo* pDynamicState;

        std::vector<VkDynamicState> dynamicStates{};

        VkPolygonMode polygonMode{ VK_POLYGON_MODE_FILL };
        VkCullModeFlags cullMode{ VK_CULL_MODE_NONE };
        VkFrontFace frontFace{ VK_FRONT_FACE_COUNTER_CLOCKWISE };

        VkRenderPass renderPass;
        std::uint32_t subpass;

        VkPipeline basePipelineHandle{ VK_NULL_HANDLE };
        std::int32_t basePipelineIndex{ -1 };
    } info;
};
}
