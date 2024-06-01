#include "vulkan_pipeline.hpp"

#include "files.hpp"
#include "vulkan_shader.hpp"
#include "vulkan_utils.hpp"

#include <vulkan/vulkan_core.h>

namespace Zeus
{
bool createGraphicsVkPipeline(
    const GraphicsPipelineConfig& config,
    VkPipelineLayout& pipelineLayout,
    VkPipeline& graphicsPipeline)
{
    auto vertShaderCode = readFile("shaders/shader.vert.spv");
    auto fragShaderCode = readFile("shaders/shader.frag.spv");

    VkShaderModule vertShaderModule;
    createVkShaderModule(config.device, vertShaderCode, vertShaderModule);
    VkShaderModule fragShaderModule;
    createVkShaderModule(config.device, fragShaderCode, fragShaderModule);

    VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vertShaderModule,
        .pName = "main",
        // It allows you to specify values for shader constants
        .pSpecializationInfo = nullptr,
    };

    VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = fragShaderModule,
        .pName = "main",
    };

    VkPipelineShaderStageCreateInfo shaderStages[] = {
        vertShaderStageCreateInfo,
        fragShaderStageCreateInfo,
    };

    // auto bindingDescription = Vertex::getBindingDescription();
    // auto attributeDescriptions = Vertex::getAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &config.bindingDescription,
        .vertexAttributeDescriptionCount =
            static_cast<std::uint32_t>(config.attributeDescriptions.size()),
        .pVertexAttributeDescriptions = config.attributeDescriptions.data(),
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };

    // specification states that dimensions of the viewport cannot exceed the
    // dimensions of the attachments that we are rendering into.
    // When specifying viewport coordinates, remember that the origin is
    // different than in OpenGL. Here we specify the upper-left corner of the
    // viewport (not the lower left). Also worth noting is that the minDepth and
    // maxDepth values must be between 0.0 and 1.0 (inclusive) but maxDepth can
    // be lower than minDepth. This will cause the depth to be calculated in
    // “reverse.”

    // x – Left side of the viewport.
    // y – Upper side of the viewport.
    // VkViewport viewport{};
    // viewport.x = 0.0f;
    // viewport.y = 0.0f;
    // viewport.width = (float)swapChainExtent.width;
    // viewport.height = (float)swapChainExtent.height;
    // viewport.minDepth = 0.0f;
    // viewport.maxDepth = 1.0f;
    //
    // VkRect2D scissor{};
    // scissor.offset = {0, 0};
    // scissor.extent = swapChainExtent;
    // VkPipelineViewportStateCreateInfo viewportState{};
    // viewportState.sType =
    //     VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    // viewportState.viewportCount = 1;
    // viewportState.pViewports = &viewport;
    // viewportState.scissorCount = 1;
    // viewportState.pScissors = &scissor;
    // We are also allowed to specify more viewports, but then the multiViewport
    // feature must be also enabled.
    VkPipelineViewportStateCreateInfo viewportStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1,
    };

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_NONE,                // VK_CULL_MODE_BACK_BIT;
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE, // VK_FRONT_FACE_CLOCKWISE
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f,
        .lineWidth = 1.0f,
    };

    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = config.msaaSamples,
        .sampleShadingEnable = VK_TRUE, // VK_FALSE
        .minSampleShading = .2f, // 1.0f, min fraction for sample shading;
                                 // closer to one is smooth
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
    };

    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE,
    };

    VkPipelineColorBlendAttachmentState colorBlendAttachmentState{
        .blendEnable = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachmentState,
        .blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f },
        // .blendConstants[0] = 0.0f,
        // .blendConstants[1] = 0.0f,
        // .blendConstants[2] = 0.0f,
        // .blendConstants[3] = 0.0f,
    };

    // If we specify that a given state is dynamic, parameters in a pipeline
    // creation info structure that are related to that state are ignored. We
    // must set the given state using the proper Vulkan commands during
    // rendering because initial values of such state may be undefined.
    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = static_cast<std::uint32_t>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data(),
    };

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &config.descriptorSetLayout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr,
    };

    VkResult pipelineLayoutResult{ vkCreatePipelineLayout(
        config.device,
        &pipelineLayoutCreateInfo,
        nullptr,
        &pipelineLayout) };

    if (pipelineLayoutResult != VK_SUCCESS)
    {
        error(
            "Failed to create pipeline layout. {}",
            vkResultToString(pipelineLayoutResult));
        return false;
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = shaderStages,
        .pVertexInputState = &vertexInputStateCreateInfo,
        .pInputAssemblyState = &inputAssemblyStateCreateInfo,
        .pViewportState = &viewportStateCreateInfo,
        .pRasterizationState = &rasterizationStateCreateInfo,
        .pMultisampleState = &multisampleStateCreateInfo,
        .pDepthStencilState = &depthStencilStateCreateInfo,
        .pColorBlendState = &colorBlendStateCreateInfo,
        .pDynamicState = &dynamicStateCreateInfo,
        .layout = pipelineLayout,
        .renderPass = config.renderPass,
        .subpass = 0,
        // When we are creating a new pipeline, we can inherit some of the
        // parameters from another one. This means that both pipelines should
        // have much in common. A good example is shader code. We don’t specify
        // what fields are the same, but the general message that the pipeline
        // inherits from another one may substantially accelerate pipeline
        // creation. But why are there two fields to indicate a “parent”
        // pipeline? We can’t use them both—only one of them at a time. When we
        // are using a handle, this means that the “parent” pipeline is already
        // created and we are deriving from the one we have provided the handle
        // of. But the pipeline creation function allows us to create many
        // pipelines at once. Using the second parameter, “parent” pipeline
        // index, we can create both “parent” and “child” pipelines in the same
        // call. We just specify an array of graphics pipeline creation info
        // structures and this array is provided to pipeline creation function.
        // So the “basePipelineIndex” is the index of pipeline creation info in
        // this very array. We just have to remember that the “parent” pipeline
        // must be earlier (must have a smaller index) in this array and it must
        // be created with the “allow derivatives” flag set.
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    VkResult result{ vkCreateGraphicsPipelines(
        config.device,
        VK_NULL_HANDLE,
        1,
        &pipelineInfo,
        nullptr,
        &graphicsPipeline) };

    if (result != VK_SUCCESS)
    {
        error(
            "Failed to create graphics pipeline. {}",
            vkResultToString(result));
        return false;
    }

    vkDestroyShaderModule(config.device, fragShaderModule, nullptr);
    vkDestroyShaderModule(config.device, vertShaderModule, nullptr);

    return true;
}
}
