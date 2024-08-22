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

    void addShaderStage(VkShaderStageFlagBits stage, VkShaderModule module);

    void addInputBindingDescription(
        std::uint32_t binding,
        std::uint32_t stride,
        VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX);

    void addInputAttributeDescription(
        std::uint32_t location,
        std::uint32_t binding,
        VkFormat format,
        std::uint32_t offset);

    void setDynamicStates(std::vector<VkDynamicState> dynamicStates);
    void setRenderPass(VkRenderPass renderPass, std::uint32_t subpass = 0);
    void setPolygonMode(VkPolygonMode polygonMode);
    void setCullMode(VkCullModeFlags cullMode, VkFrontFace frontFace);

    void clear();

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
