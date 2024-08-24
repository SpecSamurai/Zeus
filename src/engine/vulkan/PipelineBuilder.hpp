#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
class PipelineBuilder
{
public:
    PipelineBuilder();

    VkPipeline build(VkDevice device, VkPipelineLayout pipelineLayout);

    void addShaderStage(VkShaderStageFlagBits stage, VkShaderModule module);
    void setDynamicStates(std::vector<VkDynamicState> dynamicStates);
    void setRenderPass(VkRenderPass renderPass, std::uint32_t subpass = 0);
    void setPolygonMode(VkPolygonMode polygonMode);
    void setCullMode(VkCullModeFlags cullMode, VkFrontFace frontFace);
    void setPrimitiveTopology(VkPrimitiveTopology topology);

    void dynamicRendering(bool value = true);

    void setColorAttachmentFormat(VkFormat format);

    void disableDepthTest();
    void enableDepthTest(bool depthWriteEnable, VkCompareOp depthCompareOp);
    void setDepthFormat(VkFormat format);
    void setStencilFormat(VkFormat format);

    void disableBlending();
    void enableAdditiveBlending();
    void enableAlphaBlending();

    void disableMiltisampling();

    void addInputBindingDescription(
        std::uint32_t binding,
        std::uint32_t stride,
        VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX);

    void addInputAttributeDescription(
        std::uint32_t location,
        std::uint32_t binding,
        VkFormat format,
        std::uint32_t offset);

    void clear();
    void clearShaderStages();

private:
    struct PipelineCreateInfo
    {
        bool dynamicRendering{ false };

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};
        std::vector<VkVertexInputBindingDescription>
            vertexInputBindingDescription{};
        std::vector<VkVertexInputAttributeDescription>
            vertexInputAttributeDescription{};

        VkPipelineRenderingCreateInfo pipelineRendering;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState;
        VkPipelineTessellationStateCreateInfo tessellationState;
        VkPipelineViewportStateCreateInfo viewportState;
        // VkPipelineRasterizationStateCreateInfo rasterizationState;
        VkPipelineMultisampleStateCreateInfo multisampleState;
        VkPipelineDepthStencilStateCreateInfo depthStencilState;
        VkPipelineColorBlendAttachmentState colorBlendAttachmentState;
        // VkPipelineDynamicStateCreateInfo dynamicState;
        std::vector<VkDynamicState> dynamicStates{};

        VkFormat colorAttachmentformat;

        VkPolygonMode polygonMode{ VK_POLYGON_MODE_FILL };
        VkCullModeFlags cullMode{ VK_CULL_MODE_NONE };
        VkFrontFace frontFace{ VK_FRONT_FACE_COUNTER_CLOCKWISE };

        VkRenderPass renderPass{ VK_NULL_HANDLE };
        std::uint32_t subpass{ 0 };

        VkPipeline basePipelineHandle{ VK_NULL_HANDLE };
        std::int32_t basePipelineIndex{ -1 };
    } info;
};
}
