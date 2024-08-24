#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>

namespace Zeus
{
VkResult createComputePipeline(
    VkPipeline& pipeline,
    VkDevice device,
    VkPipelineLayout layout,
    VkShaderModule module,
    VkPipelineCreateFlags flags = 0,
    VkPipeline basePipelineHandle = VK_NULL_HANDLE,
    std::int32_t basePipelineIndex = -1);

VkResult createVkPipelineLayout(
    VkPipelineLayout& pipelineLayout,
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

VkPipelineVertexInputStateCreateInfo createPipelineVertexInputStateInfo(
    std::uint32_t vertexBindingDescriptionCount,
    const VkVertexInputBindingDescription* pVertexBindingDescriptions,
    std::uint32_t vertexAttributeDescriptionCount,
    const VkVertexInputAttributeDescription* pVertexAttributeDescriptions);

VkPipelineInputAssemblyStateCreateInfo createPipelineInputAssemblyStateInfo(
    VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    VkBool32 primitiveRestartEnable = VK_FALSE);

VkPipelineRasterizationStateCreateInfo createPipelineRasterizationStateInfo(
    VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL,
    VkCullModeFlags cullMode = VK_CULL_MODE_NONE,
    VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE);

VkPipelineViewportStateCreateInfo createPipelineViewportStateInfo(
    std::uint32_t viewportCount = 1,
    const VkViewport* pViewports = nullptr,
    std::uint32_t scissorCount = 1,
    const VkRect2D* pScissors = nullptr);

VkPipelineDynamicStateCreateInfo createPipelineDynamicStateCreateInfo(
    std::uint32_t dynamicStateCount,
    const VkDynamicState* pDynamicStates);
}
