#include "Pipeline.hpp"

#include "VkContext.hpp"
#include "vulkan/vulkan_debug.hpp"

#include <vulkan/vulkan_core.h>

#include <array>
#include <cassert>
#include <cstdint>
#include <format>
#include <string>
#include <string_view>
#include <vector>

namespace Zeus
{
Pipeline::Pipeline(
    std::string_view name,
    const RasterizationState& rasterizationState,
    const DepthStencilState& depthStencilState,
    const BlendState& blendState,
    VkPrimitiveTopology primitiveTopology,
    std::vector<const Shader*> shaders,
    const std::vector<const DescriptorSetLayout*>& descriptorSetLayouts,
    const std::vector<VkPushConstantRange>& pushConstants,
    const std::vector<VkFormat>& colorAttachmentFormats,
    VkFormat depthAttachmentFormat,
    VkFormat stencilAttachmentFormat)
    : m_name{ name },
      m_shaders{ shaders },
      m_rasterizationState{ rasterizationState },
      m_depthStencilState{ depthStencilState },
      m_blendState{ blendState },
      m_colorAttachmentFormats{ colorAttachmentFormats },
      m_depthAttachmentFormat{ depthAttachmentFormat },
      m_stencilAttachmentFormat{ stencilAttachmentFormat },
      m_primitiveTopology{ primitiveTopology }
{
    std::vector<VkDescriptorSetLayout> layouts(descriptorSetLayouts.size());
    for (std::uint32_t i{ 0 }; i < descriptorSetLayouts.size(); ++i)
    {
        layouts[i] = descriptorSetLayouts[i]->GetHandle();
    }

    VkPipelineLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.setLayoutCount = static_cast<std::uint32_t>(layouts.size());
    createInfo.pSetLayouts = layouts.data();
    createInfo.pushConstantRangeCount =
        static_cast<std::uint32_t>(pushConstants.size());
    createInfo.pPushConstantRanges = pushConstants.data();

    VKCHECK(
        vkCreatePipelineLayout(
            VkContext::LogicalDevice(),
            &createInfo,
            allocationCallbacks.get(),
            &m_pipelineLayout),
        "Failed to create pipeline layout.");

    if (IsGraphics())
    {
        CreateGraphicsPipeline();
    }
    else if (IsCompute())
    {
        CreateComputePipeline();
    }

    VkContext::SetDebugName(VK_OBJECT_TYPE_PIPELINE, m_handle, m_name);
    VkContext::SetDebugName(
        VK_OBJECT_TYPE_PIPELINE_LAYOUT,
        m_pipelineLayout,
        m_name.empty() ? "" : std::format("{}_Layout", m_name));
}

Pipeline::Pipeline(Pipeline&& other) noexcept
    : m_handle{ other.m_handle },
      m_pipelineLayout{ other.m_pipelineLayout },
      m_shaders{ std::move(other.m_shaders) },
      m_bindPoint{ other.m_bindPoint },
      m_rasterizationState{ other.m_rasterizationState },
      m_depthStencilState{ other.m_depthStencilState },
      m_blendState{ other.m_blendState },
      m_colorAttachmentFormats{ std::move(other.m_colorAttachmentFormats) },
      m_depthAttachmentFormat{ other.m_depthAttachmentFormat },
      m_stencilAttachmentFormat{ other.m_stencilAttachmentFormat },
      m_primitiveTopology{ other.m_primitiveTopology },
      m_instancing{ other.m_instancing }
{
    other.m_handle = VK_NULL_HANDLE;
    other.m_pipelineLayout = VK_NULL_HANDLE;
}

Pipeline& Pipeline::operator=(Pipeline&& other)
{
    if (this != &other)
    {
        if (m_handle != VK_NULL_HANDLE)
        {
            Destroy();
        }

        m_handle = other.m_handle;
        m_pipelineLayout = other.m_pipelineLayout;
        m_shaders = std::move(other.m_shaders);
        m_name = other.m_name;
        m_bindPoint = other.m_bindPoint;
        m_rasterizationState = other.m_rasterizationState;
        m_depthStencilState = other.m_depthStencilState;
        m_blendState = other.m_blendState;
        m_colorAttachmentFormats = std::move(other.m_colorAttachmentFormats);
        m_depthAttachmentFormat = other.m_depthAttachmentFormat;
        m_stencilAttachmentFormat = other.m_stencilAttachmentFormat;
        m_primitiveTopology = other.m_primitiveTopology;
        m_instancing = other.m_instancing;

        other.m_handle = VK_NULL_HANDLE;
        other.m_pipelineLayout = VK_NULL_HANDLE;
    }

    return *this;
}

Pipeline::~Pipeline()
{
    if (m_handle != VK_NULL_HANDLE)
    {
        VkContext::DeletionQueue().Add(ResourceType::Pipeline, m_handle);
        m_handle = VK_NULL_HANDLE;
    }

    if (m_pipelineLayout != VK_NULL_HANDLE)
    {
        VkContext::DeletionQueue().Add(
            ResourceType::PipelineLayout,
            m_pipelineLayout);
        m_pipelineLayout = VK_NULL_HANDLE;
    }
}

void Pipeline::Destroy()
{
    vkDestroyPipeline(
        VkContext::LogicalDevice(),
        m_handle,
        allocationCallbacks.get());
    m_handle = VK_NULL_HANDLE;

    vkDestroyPipelineLayout(
        VkContext::LogicalDevice(),
        m_pipelineLayout,
        allocationCallbacks.get());
    m_pipelineLayout = VK_NULL_HANDLE;
}

VkPipeline Pipeline::GetHandle() const
{
    return m_handle;
}

VkPipelineLayout Pipeline::GetLayout() const
{
    return m_pipelineLayout;
}

std::string_view Pipeline::GetName() const
{
    return m_name;
}

VkPipelineBindPoint Pipeline::GetBindPoint() const
{
    return m_bindPoint;
}

bool Pipeline::IsGraphics() const
{
    return m_shaders.size() >= 2;
}

bool Pipeline::IsCompute() const
{
    return m_shaders.size() == 1 &&
           m_shaders.front()->GetShaderStage() == VK_SHADER_STAGE_COMPUTE_BIT;
}

bool Pipeline::HasTessellation() const
{
    return false;
}

const std::vector<const Shader*>& Pipeline::GetShaders() const
{
    return m_shaders;
}

void Pipeline::CreateGraphicsPipeline()
{
    assert(IsGraphics());

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};
    std::vector<VkVertexInputBindingDescription> inputBindings{};
    std::vector<VkVertexInputAttributeDescription> inputAttributes{};

    for (const auto& shader : m_shaders)
    {
        shaderStages.emplace_back(CreatePipelineShaderStageInfo(
            shader->GetShaderStage(),
            shader->GetHandle(),
            shader->GetEntryPoint()));

        if (shader->GetShaderStage() == VK_SHADER_STAGE_VERTEX_BIT)
        {
            inputBindings.reserve(shader->GetVertexInputs().size());
            for (const auto& input : shader->GetVertexInputs())
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
    inputAssemblyState.topology = m_primitiveTopology;
    inputAssemblyState.primitiveRestartEnable = VK_FALSE;

    VkPipelineRasterizationStateCreateInfo rasterizationState{};
    rasterizationState.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationState.depthClampEnable =
        m_rasterizationState.GetDepthClampEnable();
    rasterizationState.rasterizerDiscardEnable = VK_FALSE;
    rasterizationState.polygonMode = m_rasterizationState.GetPolygonMode();
    rasterizationState.cullMode = m_rasterizationState.GetCullMode();
    rasterizationState.frontFace = m_rasterizationState.GetFrontFace();
    rasterizationState.depthBiasEnable =
        m_rasterizationState.GetDepthBiasClamp() != 0.f ? VK_TRUE : VK_FALSE;
    rasterizationState.depthBiasConstantFactor =
        m_rasterizationState.GetDepthBiasConstantFactor();
    rasterizationState.depthBiasClamp =
        m_rasterizationState.GetDepthBiasClamp();
    rasterizationState.depthBiasSlopeFactor =
        m_rasterizationState.GetDepthBiasSlopeFactor();
    rasterizationState.lineWidth = m_rasterizationState.GetLineWidth();

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

    VkPipelineDepthStencilStateCreateInfo depthStencilState{};
    depthStencilState.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilState.depthTestEnable =
        m_depthStencilState.GetDepthTestEnable();
    depthStencilState.depthWriteEnable =
        m_depthStencilState.GetDepthWriteEnable();
    depthStencilState.depthCompareOp = m_depthStencilState.GetDepthCompareOp();
    depthStencilState.depthBoundsTestEnable =
        m_depthStencilState.GetDepthBoundsTestEnable();
    depthStencilState.stencilTestEnable =
        m_depthStencilState.GetStencilTestEnable();
    depthStencilState.front = {};
    depthStencilState.back = {};
    depthStencilState.minDepthBounds = 0.f;
    depthStencilState.maxDepthBounds = 1.f;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.blendEnable = m_blendState.GetBlendEnable();
    colorBlendAttachment.srcColorBlendFactor =
        m_blendState.GetSrcColorBlendFactor();
    colorBlendAttachment.dstColorBlendFactor =
        m_blendState.GetDstColorBlendFactor();
    colorBlendAttachment.colorBlendOp = m_blendState.GetColorBlendOp();
    colorBlendAttachment.srcAlphaBlendFactor =
        m_blendState.GetSrcAlphaBlendFactor();
    colorBlendAttachment.dstAlphaBlendFactor =
        m_blendState.GetDstAlphaBlendFactor();
    colorBlendAttachment.alphaBlendOp = m_blendState.GetAlphaBlendOp();
    colorBlendAttachment.colorWriteMask = m_blendState.GetColorWriteMask();

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

    VkPipelineRenderingCreateInfo pipelineRendering{};
    pipelineRendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    pipelineRendering.viewMask = 0;
    pipelineRendering.colorAttachmentCount =
        static_cast<std::uint32_t>(m_colorAttachmentFormats.size());
    pipelineRendering.pColorAttachmentFormats = m_colorAttachmentFormats.data();
    pipelineRendering.depthAttachmentFormat = m_depthAttachmentFormat;
    pipelineRendering.stencilAttachmentFormat = m_stencilAttachmentFormat;

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
            VkContext::LogicalDevice(),
            VK_NULL_HANDLE,
            1,
            &pipelineCreateInfo,
            allocationCallbacks.get(),
            &m_handle),
        "Failed to create pipeline.");

    m_bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
}

void Pipeline::CreateComputePipeline()
{
    assert(IsCompute());

    const auto& shader{ m_shaders.front() };

    VkPipelineShaderStageCreateInfo stageCreateInfo{
        CreatePipelineShaderStageInfo(
            shader->GetShaderStage(),
            shader->GetHandle(),
            shader->GetEntryPoint())
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
            VkContext::LogicalDevice(),
            VK_NULL_HANDLE,
            1,
            &createInfo,
            allocationCallbacks.get(),
            &m_handle),
        "Failed to create compute pipeline layout.");

    m_bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
}

VkPipelineShaderStageCreateInfo Pipeline::CreatePipelineShaderStageInfo(
    VkShaderStageFlagBits stage,
    VkShaderModule module,
    const char* pName) const
{
    return VkPipelineShaderStageCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
        .stage = stage,
        .module = module,
        .pName = pName,
        .pSpecializationInfo = nullptr,
    };
}
}
