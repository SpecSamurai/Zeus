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
    createInfo.flags = 0;
    createInfo.stage = stage;
    createInfo.module = module;
    createInfo.pName = pName;
    createInfo.pSpecializationInfo = pSpecializationInfo;

    return createInfo;
}
}
