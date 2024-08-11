#include "vulkan_pipeline.hpp"

#include "vulkan_utils.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>

namespace Zeus
{
VkResult createVkPipelineLayout(
    VkPipelineLayout& pipelineLayout,
    VkDevice device,
    std::uint32_t setLayoutCount,
    const VkDescriptorSetLayout* pSetLayouts,
    std::uint32_t pushConstantRangeCount,
    const VkPushConstantRange* pPushConstantRanges)
{
    VkPipelineLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.setLayoutCount = setLayoutCount;
    createInfo.pSetLayouts = pSetLayouts;
    createInfo.pushConstantRangeCount = pushConstantRangeCount;
    createInfo.pPushConstantRanges = pPushConstantRanges;

    VkResult result{
        vkCreatePipelineLayout(device, &createInfo, nullptr, &pipelineLayout)
    };

    VKCHECK(result, "Failed to create pipeline layout.");

    return result;
}

VkPipelineShaderStageCreateInfo createPipelineShaderStageInfo(
    VkShaderStageFlagBits stage,
    VkShaderModule module,
    const char* pName,
    const VkSpecializationInfo* pSpecializationInfo)
{
    VkPipelineShaderStageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo.stage = stage;
    createInfo.module = module;
    createInfo.pName = pName;
    createInfo.pSpecializationInfo = pSpecializationInfo;

    return createInfo;
}

VkPipelineVertexInputStateCreateInfo createPipelineVertexInputStateInfo(
    std::uint32_t vertexBindingDescriptionCount,
    const VkVertexInputBindingDescription* pVertexBindingDescriptions,
    std::uint32_t vertexAttributeDescriptionCount,
    const VkVertexInputAttributeDescription* pVertexAttributeDescriptions)
{
    VkPipelineVertexInputStateCreateInfo createInfo{};
    createInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,

    createInfo.vertexBindingDescriptionCount = vertexBindingDescriptionCount;
    createInfo.pVertexBindingDescriptions = pVertexBindingDescriptions;
    createInfo.vertexAttributeDescriptionCount =
        vertexAttributeDescriptionCount;
    createInfo.pVertexAttributeDescriptions = pVertexAttributeDescriptions;

    return createInfo;
}

VkPipelineInputAssemblyStateCreateInfo createPipelineInputAssemblyStateInfo(
    VkPrimitiveTopology topology,
    VkBool32 primitiveRestartEnable)
{
    VkPipelineInputAssemblyStateCreateInfo createInfo{};
    createInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    createInfo.topology = topology;
    createInfo.primitiveRestartEnable = primitiveRestartEnable;

    return createInfo;
}

VkPipelineRasterizationStateCreateInfo createPipelineRasterizationStateInfo(
    VkPolygonMode polygonMode,
    VkCullModeFlags cullMode,
    VkFrontFace frontFace)
{
    VkPipelineRasterizationStateCreateInfo createInfo{};
    createInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    createInfo.depthClampEnable = VK_FALSE;
    createInfo.rasterizerDiscardEnable = VK_FALSE;
    createInfo.polygonMode = polygonMode;
    createInfo.cullMode = cullMode;
    createInfo.frontFace = frontFace;
    createInfo.depthBiasEnable = VK_FALSE;
    createInfo.depthBiasConstantFactor = 0.0f;
    createInfo.depthBiasClamp = 0.0f;
    createInfo.depthBiasSlopeFactor = 0.0f;
    createInfo.lineWidth = 1.0f;

    return createInfo;
}

VkPipelineViewportStateCreateInfo createPipelineViewportStateInfo(
    std::uint32_t viewportCount,
    const VkViewport* pViewports,
    std::uint32_t scissorCount,
    const VkRect2D* pScissors)
{
    // x – Left side of the viewport.
    // y – Upper side of the viewport.
    // VkViewport viewport{};
    // viewport.x = 0.0f;
    // viewport.y = 0.0f;
    // viewport.width = (float)swapchain.extent.width;
    // viewport.height = (float)swapchain.extent.height;
    // viewport.minDepth = 0.0f;
    // viewport.maxDepth = 1.0f;
    //
    // VkRect2D scissor{};
    // scissor.offset = { 0, 0 };
    // scissor.extent = swapchain.extent;
    //
    // VkPipelineViewportStateCreateInfo viewportState{};
    // viewportState.sType =
    // VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    // viewportState.viewportCount = 1;
    // viewportState.pViewports = &viewport;
    // viewportState.scissorCount = 1;
    // viewportState.pScissors = &scissor;

    VkPipelineViewportStateCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    createInfo.viewportCount = viewportCount;
    createInfo.pViewports = pViewports;
    createInfo.scissorCount = scissorCount;
    createInfo.pScissors = pScissors;

    return createInfo;
}

VkPipelineMultisampleStateCreateInfo createPipelineMultisampleStateInfo(
    VkSampleCountFlagBits rasterizationSamples,
    VkBool32 sampleShadingEnable,
    float minSampleShading,
    const VkSampleMask* pSampleMask,
    VkBool32 alphaToCoverageEnable,
    VkBool32 alphaToOneEnable)
{
    VkPipelineMultisampleStateCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    createInfo.rasterizationSamples = rasterizationSamples;
    createInfo.sampleShadingEnable = VK_FALSE;
    // .sampleShadingEnable = VK_TRUE, // VK_FALSE
    //                          // closer to one is smooth
    // .minSampleShading = .2f, // 1.0f, min fraction for sample shading;
    // .pSampleMask = nullptr,
    // .alphaToCoverageEnable = VK_FALSE,
    // .alphaToOneEnable = VK_FALSE,

    return createInfo;
}

VkPipelineDepthStencilStateCreateInfo createPipelineDepthStencilStateInfo()
{
    VkPipelineDepthStencilStateCreateInfo createInfo{};
    createInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    createInfo.depthTestEnable = VK_TRUE;
    createInfo.depthWriteEnable = VK_TRUE;
    createInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    createInfo.depthBoundsTestEnable = VK_FALSE;
    createInfo.stencilTestEnable = VK_FALSE;
    createInfo.front = {};            // Optional
    createInfo.back = {};             // Optional
    createInfo.minDepthBounds = 0.0f; // Optional
    createInfo.maxDepthBounds = 1.0f; // Optional

    return createInfo;
}

VkPipelineColorBlendStateCreateInfo createPipelineColorBlendStateCreateInfo()
{
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

    VkPipelineColorBlendStateCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    createInfo.logicOpEnable = VK_FALSE;
    createInfo.logicOp = VK_LOGIC_OP_COPY;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &colorBlendAttachmentState;
    createInfo.blendConstants[0] = 0.0f;
    createInfo.blendConstants[1] = 0.0f;
    createInfo.blendConstants[2] = 0.0f;
    createInfo.blendConstants[3] = 0.0f;

    return createInfo;
}

VkPipelineDynamicStateCreateInfo createPipelineDynamicStateCreateInfo(
    std::uint32_t dynamicStateCount,
    const VkDynamicState* pDynamicStates)
{
    VkPipelineDynamicStateCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    createInfo.dynamicStateCount = dynamicStateCount;
    createInfo.pDynamicStates = pDynamicStates;

    return createInfo;
}
}
