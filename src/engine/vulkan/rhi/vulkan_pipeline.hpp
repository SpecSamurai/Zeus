#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>

namespace Zeus
{
VkResult createComputePipeline(
    VkPipeline* pipeline,
    VkDevice device,
    VkPipelineLayout layout,
    VkShaderModule module,
    VkPipelineCreateFlags flags = 0,
    VkPipeline basePipelineHandle = VK_NULL_HANDLE,
    std::int32_t basePipelineIndex = -1);

VkResult createVkPipelineLayout(
    VkPipelineLayout* pipelineLayout,
    VkDevice device,
    std::uint32_t setLayoutCount = 0,
    const VkDescriptorSetLayout* pSetLayouts = nullptr,
    std::uint32_t pushConstantRangeCount = 0,
    const VkPushConstantRange* pPushConstantRanges = nullptr);

VkPipelineShaderStageCreateInfo createPipelineShaderStageInfo(
    VkShaderStageFlagBits stage,
    VkShaderModule module,
    const char* pName = "main",
    const VkSpecializationInfo* pSpecializationInfo = nullptr);
}
