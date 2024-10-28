#include "vulkan_pipeline.hpp"

#include "vulkan_debug.hpp"
#include "vulkan_memory.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>

namespace Zeus
{
VkResult createComputePipeline(
    VkPipeline* pipeline,
    VkDevice device,
    VkPipelineLayout layout,
    VkShaderModule module,
    VkPipelineCreateFlags flags,
    VkPipeline basePipelineHandle,
    std::int32_t basePipelineIndex)
{
    VkPipelineShaderStageCreateInfo stageCreateInfo{};
    stageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stageCreateInfo.flags = 0;
    stageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    stageCreateInfo.module = module;
    stageCreateInfo.pName = "main";

    VkComputePipelineCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    createInfo.flags = flags;
    createInfo.stage = stageCreateInfo;
    createInfo.layout = layout;
    createInfo.basePipelineHandle = basePipelineHandle;
    createInfo.basePipelineIndex = basePipelineIndex;

    VkResult result{ vkCreateComputePipelines(
        device,
        VK_NULL_HANDLE,
        1,
        &createInfo,
        allocationCallbacks.get(),
        pipeline) };

    VKCHECK(result, "Failed to create compute pipeline layout.");

    return result;
}

VkResult createVkPipelineLayout(
    VkPipelineLayout* pipelineLayout,
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

    VkResult result{ vkCreatePipelineLayout(
        device,
        &createInfo,
        allocationCallbacks.get(),
        pipelineLayout) };

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
    VkPipelineViewportStateCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    createInfo.viewportCount = viewportCount;
    createInfo.pViewports = pViewports;
    createInfo.scissorCount = scissorCount;
    createInfo.pScissors = pScissors;

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
