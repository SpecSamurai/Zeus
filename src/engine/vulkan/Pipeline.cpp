#include "Pipeline.hpp"

#include "Definitions.hpp"
#include "DeletionQueue.hpp"
#include "VkContext.hpp"
#include "rhi/vulkan_debug.hpp"
#include "rhi/vulkan_pipeline.hpp"

#include <vulkan/vulkan_core.h>

#include <array>
#include <cassert>
#include <cstdint>
#include <vector>

namespace Zeus
{
Pipeline::Pipeline(
    const PipelineState& pipelineState,
    const std::vector<DescriptorSetLayout>& descriptorSetLayouts,
    const std::vector<PushConstants>& pushConstants,
    const char* name)
    : m_state{ pipelineState },
      m_name(name)
{
    std::vector<VkDescriptorSetLayout> layouts(descriptorSetLayouts.size());
    for (std::uint32_t i{ 0 }; i < descriptorSetLayouts.size(); ++i)
    {
        layouts[i] = descriptorSetLayouts[i].GetHandle();
    }

    std::vector<VkPushConstantRange> pushConstantRanges(pushConstants.size());
    for (std::uint32_t i{ 0 }; i < pushConstants.size(); ++i)
    {
        pushConstantRanges[i] = VkPushConstantRange{
            .stageFlags = pushConstants[i].GetStageFlags(),
            .offset = pushConstants[i].GetOffset(),
            .size = pushConstants[i].GetSize(),
        };
    }

    createVkPipelineLayout(
        &m_pipelineLayout,
        VkContext::GetLogicalDevice(),
        static_cast<std::uint32_t>(layouts.size()),
        layouts.data(),
        static_cast<std::uint32_t>(pushConstantRanges.size()),
        pushConstantRanges.data());

    if (m_state.IsGraphics())
    {
        CreateGraphicsPipeline();
    }
    else if (m_state.IsCompute())
    {
        CreateComputePipeline();
    }

    VkContext::SetDebugName(VK_OBJECT_TYPE_PIPELINE, m_handle, m_name);
    VkContext::SetDebugName(
        VK_OBJECT_TYPE_PIPELINE_LAYOUT,
        m_pipelineLayout,
        m_name);
}

Pipeline::~Pipeline()
{
    if (m_handle != VK_NULL_HANDLE)
    {
        VkContext::GetDeletionQueue().Add(ResourceType::Pipeline, m_handle);
        m_handle = VK_NULL_HANDLE;
    }

    if (m_pipelineLayout != VK_NULL_HANDLE)
    {
        VkContext::GetDeletionQueue().Add(
            ResourceType::PipelineLayout,
            m_pipelineLayout);
        m_pipelineLayout = VK_NULL_HANDLE;
    }
}

void Pipeline::Destroy()
{
    vkDestroyPipeline(
        VkContext::GetLogicalDevice(),
        m_handle,
        allocationCallbacks.get());
    m_handle = VK_NULL_HANDLE;

    vkDestroyPipelineLayout(
        VkContext::GetLogicalDevice(),
        m_pipelineLayout,
        allocationCallbacks.get());
    m_pipelineLayout = VK_NULL_HANDLE;

    m_name = nullptr;
}

VkPipeline Pipeline::GetHandle() const
{
    return m_handle;
}

VkPipelineLayout Pipeline::GetLayout() const
{
    return m_pipelineLayout;
}

const PipelineState& Pipeline::GetState() const
{
    return m_state;
}

const char* Pipeline::GetName() const
{
    return m_name;
}

void Pipeline::CreateGraphicsPipeline()
{
    assert(m_state.IsGraphics());

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};
    std::vector<VkVertexInputBindingDescription> inputBindings{};
    std::vector<VkVertexInputAttributeDescription> inputAttributes{};

    for (const auto& shader : m_state.GetShaders())
    {
        shaderStages.emplace_back(createPipelineShaderStageInfo(
            shader.GetShaderStage(),
            shader.GetHandle(),
            shader.GetEntryPoint().c_str()));

        if (shader.GetShaderStage() == VK_SHADER_STAGE_VERTEX_BIT)
        {
            inputBindings.reserve(shader.GetVertexInputs().size());
            for (const auto& input : shader.GetVertexInputs())
            {
                inputBindings.emplace_back(VkVertexInputBindingDescription{
                    .binding = input.GetBinding(),
                    .stride = input.GetStride(),
                    .inputRate = input.GetInputRate(),
                });

                inputAttributes.reserve(input.GetAttributes().size());
                for (const auto& attribute : input.GetAttributes())
                {
                    inputAttributes.emplace_back(
                        VkVertexInputAttributeDescription{
                            .location = attribute.GetLocation(),
                            .binding = attribute.GetBinding(),
                            .format = attribute.GetFormat(),
                            .offset = attribute.GetOffset(),
                        });
                }
            }
        }
    }

    VkPipelineVertexInputStateCreateInfo vertexInputState{};
    vertexInputState.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputState.vertexBindingDescriptionCount =
        static_cast<std::uint32_t>(inputBindings.size());
    vertexInputState.pVertexBindingDescriptions = inputBindings.data();
    vertexInputState.vertexAttributeDescriptionCount =
        static_cast<std::uint32_t>(inputAttributes.size());
    vertexInputState.pVertexAttributeDescriptions = inputAttributes.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
    inputAssemblyState.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.topology = m_state.GetPrimitiveTopology();
    inputAssemblyState.primitiveRestartEnable = VK_FALSE;

    const auto& rasterState{ m_state.GetRasterizationState() };
    VkPipelineRasterizationStateCreateInfo rasterizationState{};
    rasterizationState.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationState.depthClampEnable = rasterState.GetDepthClampEnable();
    rasterizationState.rasterizerDiscardEnable = VK_FALSE;
    rasterizationState.polygonMode = rasterState.GetPolygonMode();
    rasterizationState.cullMode = rasterState.GetCullMode();
    rasterizationState.frontFace = rasterState.GetFrontFace();
    rasterizationState.depthBiasEnable =
        rasterState.GetDepthBiasClamp() != 0.f ? VK_TRUE : VK_FALSE;
    rasterizationState.depthBiasConstantFactor =
        rasterState.GetDepthBiasConstantFactor();
    rasterizationState.depthBiasClamp = rasterState.GetDepthBiasClamp();
    rasterizationState.depthBiasSlopeFactor =
        rasterState.GetDepthBiasSlopeFactor();
    rasterizationState.lineWidth = rasterState.GetLineWidth();

    std::array<VkDynamicState, 2> dynamicStates{
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_VIEWPORT,
        // VK_DYNAMIC_STATE_CULL_MODE,
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount =
        static_cast<std::uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = nullptr;
    viewportState.scissorCount = 1;
    viewportState.pScissors = nullptr;

    VkPipelineMultisampleStateCreateInfo multisampleState{};
    multisampleState.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleState.sampleShadingEnable = VK_FALSE;
    multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleState.minSampleShading = 1.0f;
    multisampleState.pSampleMask = nullptr;
    multisampleState.alphaToCoverageEnable = VK_FALSE;
    multisampleState.alphaToOneEnable = VK_FALSE;
    // .sampleShadingEnable = VK_TRUE,
    // closer to one is smooth
    // .minSampleShading = .2f, // 1.0f, min fraction for sample shading;
    // .pSampleMask = nullptr,
    // .alphaToCoverageEnable = VK_FALSE,
    // .alphaToOneEnable = VK_FALSE,

    const auto& depthStencil{ m_state.GetDepthStencilState() };
    VkPipelineDepthStencilStateCreateInfo depthStencilState{};
    depthStencilState.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilState.depthTestEnable = depthStencil.GetDepthTestEnable();
    depthStencilState.depthWriteEnable = depthStencil.GetDepthWriteEnable();
    depthStencilState.depthCompareOp = depthStencil.GetDepthCompareOp();
    depthStencilState.depthBoundsTestEnable =
        depthStencil.GetDepthBoundsTestEnable();
    depthStencilState.stencilTestEnable = depthStencil.GetStencilTestEnable();
    depthStencilState.front = {};
    depthStencilState.back = {};
    depthStencilState.minDepthBounds = 0.f;
    depthStencilState.maxDepthBounds = 1.f;

    const auto& blendState{ m_state.GetBlendState() };
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.blendEnable = blendState.GetBlendEnable();
    colorBlendAttachment.srcColorBlendFactor =
        blendState.GetSrcColorBlendFactor();
    colorBlendAttachment.dstColorBlendFactor =
        blendState.GetDstColorBlendFactor();
    colorBlendAttachment.colorBlendOp = blendState.GetColorBlendOp();
    colorBlendAttachment.srcAlphaBlendFactor =
        blendState.GetSrcAlphaBlendFactor();
    colorBlendAttachment.dstAlphaBlendFactor =
        blendState.GetDstAlphaBlendFactor();
    colorBlendAttachment.alphaBlendOp = blendState.GetAlphaBlendOp();
    colorBlendAttachment.colorWriteMask = blendState.GetColorWriteMask();

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
    colorBlendStateCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
    colorBlendStateCreateInfo.attachmentCount = 1;
    colorBlendStateCreateInfo.pAttachments = &colorBlendAttachment;
    colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
    colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
    colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
    colorBlendStateCreateInfo.blendConstants[3] = 0.0f;

    const auto& colorAttachmentFormats{ m_state.GetColorAttachmentFormats() };
    VkPipelineRenderingCreateInfo pipelineRendering{};
    pipelineRendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    pipelineRendering.viewMask = 0;
    pipelineRendering.colorAttachmentCount =
        static_cast<std::uint32_t>(colorAttachmentFormats.size());
    pipelineRendering.pColorAttachmentFormats = colorAttachmentFormats.data();
    pipelineRendering.depthAttachmentFormat =
        m_state.GetDepthAttachmentFormat();
    pipelineRendering.stencilAttachmentFormat =
        m_state.GetStencilAttachmentFormat();

    VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.pNext = &pipelineRendering;
    // pipelineCreateInfo.renderPass = renderPass;
    // pipelineCreateInfo.subpass = subpass;
    pipelineCreateInfo.stageCount =
        static_cast<std::uint32_t>(shaderStages.size());
    pipelineCreateInfo.pStages = shaderStages.data();
    pipelineCreateInfo.pVertexInputState = &vertexInputState;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pTessellationState = nullptr;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
    pipelineCreateInfo.pDynamicState = &dynamicState;

    pipelineCreateInfo.layout = m_pipelineLayout;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = -1;

    VKCHECK(
        vkCreateGraphicsPipelines(
            VkContext::GetLogicalDevice(),
            VK_NULL_HANDLE,
            1,
            &pipelineCreateInfo,
            allocationCallbacks.get(),
            &m_handle),
        "Failed to create pipeline.");
}

void Pipeline::CreateComputePipeline()
{
    assert(m_state.IsCompute());

    const auto& shader{ m_state.GetShaders().front() };
    VkPipelineShaderStageCreateInfo stageCreateInfo{
        createPipelineShaderStageInfo(
            shader.GetShaderStage(),
            shader.GetHandle(),
            shader.GetEntryPoint().c_str())
    };

    VkComputePipelineCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    createInfo.flags = 0;
    createInfo.stage = stageCreateInfo;
    createInfo.layout = m_pipelineLayout;
    createInfo.basePipelineHandle = VK_NULL_HANDLE;
    createInfo.basePipelineIndex = -1;

    VKCHECK(
        vkCreateComputePipelines(
            VkContext::GetLogicalDevice(),
            VK_NULL_HANDLE,
            1,
            &createInfo,
            allocationCallbacks.get(),
            &m_handle),
        "Failed to create compute pipeline layout.");
}
}
