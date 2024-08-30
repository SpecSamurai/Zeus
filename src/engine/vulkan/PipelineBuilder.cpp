#include "PipelineBuilder.hpp"

#include "vulkan_debug.hpp"
#include "vulkan_memory.hpp"
#include "vulkan_pipeline.hpp"

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

    VkPipelineRasterizationStateCreateInfo rasterizationState{
        createPipelineRasterizationStateInfo(
            info.polygonMode,
            info.cullMode,
            info.frontFace)
    };

    VkPipelineDynamicStateCreateInfo dynamicState{
        createPipelineDynamicStateCreateInfo(
            static_cast<std::uint32_t>(info.dynamicStates.size()),
            info.dynamicStates.data())
    };

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
    colorBlendStateCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
    colorBlendStateCreateInfo.attachmentCount = 1;
    colorBlendStateCreateInfo.pAttachments = &info.colorBlendAttachmentState;
    colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
    colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
    colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
    colorBlendStateCreateInfo.blendConstants[3] = 0.0f;

    VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    if (info.dynamicRendering)
    {
        pipelineCreateInfo.pNext = &info.pipelineRendering;
    }
    else
    {
        pipelineCreateInfo.renderPass = info.renderPass;
        pipelineCreateInfo.subpass = info.subpass;
    }

    pipelineCreateInfo.stageCount =
        static_cast<std::uint32_t>(info.shaderStages.size());
    pipelineCreateInfo.pStages = info.shaderStages.data();
    pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &info.inputAssemblyState;
    pipelineCreateInfo.pViewportState = &info.viewportState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pMultisampleState = &info.multisampleState;
    pipelineCreateInfo.pDepthStencilState = &info.depthStencilState;
    pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
    pipelineCreateInfo.pDynamicState = &dynamicState;

    pipelineCreateInfo.layout = pipelineLayout;
    pipelineCreateInfo.basePipelineHandle = info.basePipelineHandle;
    pipelineCreateInfo.basePipelineIndex = info.basePipelineIndex;

    VkPipeline pipeline;
    VkResult result{
        vkCreateGraphicsPipelines(
            device,
            VK_NULL_HANDLE,
            1,
            &pipelineCreateInfo,
            allocationCallbacks.get(),
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

void PipelineBuilder::setPrimitiveTopology(VkPrimitiveTopology topology)
{
    info.inputAssemblyState = createPipelineInputAssemblyStateInfo(topology);
}

void PipelineBuilder::dynamicRendering(bool value)
{
    info.dynamicRendering = value;

    info.pipelineRendering.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    info.pipelineRendering.viewMask = 0;
}

void PipelineBuilder::setColorAttachmentFormat(VkFormat format)
{
    info.colorAttachmentformat = format;

    info.pipelineRendering.colorAttachmentCount = 1;
    info.pipelineRendering.pColorAttachmentFormats =
        &info.colorAttachmentformat;
}

void PipelineBuilder::enableDepthTest(
    VkBool32 depthWriteEnable,
    VkCompareOp depthCompareOp)
{
    info.depthStencilState.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    info.depthStencilState.depthTestEnable = VK_TRUE;
    info.depthStencilState.depthWriteEnable = depthWriteEnable;
    info.depthStencilState.depthCompareOp = depthCompareOp;
    info.depthStencilState.depthBoundsTestEnable = VK_FALSE;
    info.depthStencilState.stencilTestEnable = VK_FALSE;
    info.depthStencilState.front = {};
    info.depthStencilState.back = {};
    info.depthStencilState.minDepthBounds = 0.f;
    info.depthStencilState.maxDepthBounds = 1.f;
}

void PipelineBuilder::setDepthFormat(VkFormat format)
{
    info.pipelineRendering.depthAttachmentFormat = format;
}

void PipelineBuilder::setStencilFormat(VkFormat format)
{
    info.pipelineRendering.stencilAttachmentFormat = format;
}

void PipelineBuilder::disableBlending()
{
    info.colorBlendAttachmentState.blendEnable = VK_FALSE;
    info.colorBlendAttachmentState.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
}

void PipelineBuilder::enableAdditiveBlending()
{
    info.colorBlendAttachmentState.blendEnable = VK_TRUE;
    info.colorBlendAttachmentState.srcColorBlendFactor =
        VK_BLEND_FACTOR_SRC_ALPHA;
    info.colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
    info.colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    info.colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    info.colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    info.colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
    info.colorBlendAttachmentState.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
}

void PipelineBuilder::enableAlphaBlending()
{
    info.colorBlendAttachmentState.blendEnable = VK_TRUE;
    info.colorBlendAttachmentState.srcColorBlendFactor =
        VK_BLEND_FACTOR_SRC_ALPHA;
    info.colorBlendAttachmentState.dstColorBlendFactor =
        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    info.colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    info.colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    info.colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    info.colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
    info.colorBlendAttachmentState.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
}

void PipelineBuilder::disableDepthTest()
{
    info.depthStencilState.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    info.depthStencilState.depthTestEnable = VK_FALSE;
    info.depthStencilState.depthWriteEnable = VK_FALSE;
    info.depthStencilState.depthCompareOp = VK_COMPARE_OP_NEVER;
    info.depthStencilState.depthBoundsTestEnable = VK_FALSE;
    info.depthStencilState.stencilTestEnable = VK_FALSE;
    info.depthStencilState.front = {};
    info.depthStencilState.back = {};
    info.depthStencilState.minDepthBounds = 0.f;
    info.depthStencilState.maxDepthBounds = 1.f;
}

void PipelineBuilder::disableMiltisampling()
{
    info.multisampleState.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

    info.multisampleState.sampleShadingEnable = VK_FALSE;
    info.multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    info.multisampleState.minSampleShading = 1.0f;
    info.multisampleState.pSampleMask = nullptr;
    info.multisampleState.alphaToCoverageEnable = VK_FALSE;
    info.multisampleState.alphaToOneEnable = VK_FALSE;

    // .sampleShadingEnable = VK_TRUE,
    // closer to one is smooth
    // .minSampleShading = .2f, // 1.0f, min fraction for sample shading;
    // .pSampleMask = nullptr,
    // .alphaToCoverageEnable = VK_FALSE,
    // .alphaToOneEnable = VK_FALSE,
}

void PipelineBuilder::clear()
{
    info.dynamicRendering = false;

    info.renderPass = VK_NULL_HANDLE;
    info.subpass = 0;

    info.basePipelineHandle = { VK_NULL_HANDLE };
    info.basePipelineIndex = { -1 };

    info.polygonMode = VK_POLYGON_MODE_FILL;
    info.cullMode = VK_CULL_MODE_NONE;
    info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

    info.colorAttachmentformat = VK_FORMAT_UNDEFINED;

    info.shaderStages.clear();
    info.vertexInputBindingDescription.clear();
    info.vertexInputAttributeDescription.clear();
    info.dynamicStates.clear();

    info.pipelineRendering = {};
    info.inputAssemblyState = {};
    info.tessellationState = {};
    info.viewportState = createPipelineViewportStateInfo();
    info.multisampleState = {};
    info.depthStencilState = {};
    info.colorBlendAttachmentState = {};
}

void PipelineBuilder::clearShaderStages()
{
    info.shaderStages.clear();
}

PipelineBuilder::PipelineBuilder()
{
    clear();
}
}
