#include "PipelineBuilder.hpp"

#include "vulkan_pipeline.hpp"
#include "vulkan_utils.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
VkPipeline PipelineBuilder::build(
    VkDevice device,
    VkPipelineLayout pipelineLayout)
{
    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{
        createPipelineVertexInputStateInfo(
            static_cast<std::uint32_t>(
                info.vertexInputBindingDescription.size()),
            info.vertexInputBindingDescription.data(),
            static_cast<std::uint32_t>(
                info.vertexInputAttributeDescription.size()),
            info.vertexInputAttributeDescription.data())
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{
        createPipelineInputAssemblyStateInfo()
    };

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo{
        createPipelineViewportStateInfo()
    };

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{
        createPipelineRasterizationStateInfo(
            info.polygonMode,
            info.cullMode,
            info.frontFace)
    };

    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{
        createPipelineMultisampleStateInfo()
    };

    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{
        createPipelineDepthStencilStateInfo()
    };

    VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
    colorBlendAttachmentState.blendEnable = VK_FALSE;
    colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
    colorBlendStateCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
    colorBlendStateCreateInfo.attachmentCount = 1;
    colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
    colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
    colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
    colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
    colorBlendStateCreateInfo.blendConstants[3] = 0.0f;

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{
        createPipelineDynamicStateCreateInfo(
            static_cast<std::uint32_t>(info.dynamicStates.size()),
            info.dynamicStates.data())
    };

    VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    pipelineCreateInfo.stageCount =
        static_cast<std::uint32_t>(info.shaderStages.size());
    pipelineCreateInfo.pStages = info.shaderStages.data();
    pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
    pipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
    pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
    pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;

    pipelineCreateInfo.layout = pipelineLayout;

    pipelineCreateInfo.renderPass = info.renderPass;
    pipelineCreateInfo.subpass = info.subpass;

    pipelineCreateInfo.basePipelineHandle = info.basePipelineHandle;
    pipelineCreateInfo.basePipelineIndex = info.basePipelineIndex;

    VkPipeline pipeline;
    VkResult result{
        vkCreateGraphicsPipelines(
            device,
            VK_NULL_HANDLE,
            1,
            &pipelineCreateInfo,
            nullptr,
            &pipeline),
    };

    VKCHECK(result, "Failed to create pipeline.");

    return pipeline;
}

void PipelineBuilder::addShaderStage(
    VkShaderStageFlagBits stage,
    VkShaderModule module)
{
    info.shaderStages.push_back(createPipelineShaderStageInfo(stage, module));
}

void PipelineBuilder::addInputBindingDescription(
    std::uint32_t binding,
    std::uint32_t stride,
    VkVertexInputRate inputRate)
{
    VkVertexInputBindingDescription description{};
    description.binding = binding;
    description.stride = stride;
    description.inputRate = inputRate;

    info.vertexInputBindingDescription.push_back(description);
}

void PipelineBuilder::addInputAttributeDescription(
    std::uint32_t location,
    std::uint32_t binding,
    VkFormat format,
    std::uint32_t offset)
{
    VkVertexInputAttributeDescription descriptions{};
    descriptions.location = location;
    descriptions.binding = binding;
    descriptions.format = format;
    descriptions.offset = offset;

    info.vertexInputAttributeDescription.push_back(descriptions);
}

void PipelineBuilder::setDynamicStates(
    std::vector<VkDynamicState> dynamicStates)
{
    info.dynamicStates = dynamicStates;
}

void PipelineBuilder::setRenderPass(
    VkRenderPass renderPass,
    std::uint32_t subpass)
{
    info.renderPass = renderPass;
    info.subpass = subpass;
}

void PipelineBuilder::setPolygonMode(VkPolygonMode polygonMode)
{
    info.polygonMode = polygonMode;
}

void PipelineBuilder::setCullMode(
    VkCullModeFlags cullMode,
    VkFrontFace frontFace)
{
    info.cullMode = cullMode;
    info.frontFace = frontFace;
}

void PipelineBuilder::clear()
{
    info.shaderStages.clear();
    info.vertexInputBindingDescription.clear();
    info.vertexInputAttributeDescription.clear();

    info.dynamicStates.clear();

    info.polygonMode = { VK_POLYGON_MODE_FILL };
    info.cullMode = { VK_CULL_MODE_NONE };
    info.frontFace = { VK_FRONT_FACE_COUNTER_CLOCKWISE };

    info.renderPass = VK_NULL_HANDLE;
    info.subpass = 0;

    info.basePipelineHandle = { VK_NULL_HANDLE };
    info.basePipelineIndex = { -1 };
}
}
