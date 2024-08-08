#pragma once

#include <vulkan/vulkan_core.h>

#include <vector>

namespace Zeus
{
struct GraphicsPipelineConfig
{
    const VkDevice& device;
    const VkRenderPass& renderPass;
    const VkDescriptorSetLayout& descriptorSetLayout;
    const VkSampleCountFlagBits msaaSamples;
    const VkVertexInputBindingDescription& bindingDescription;
    const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions;
    const std::vector<char>& vertShaderCode;
    const std::vector<char>& fragShaderCode;
    const std::vector<VkPushConstantRange>& pushConstantRanges;
};

bool createGraphicsVkPipeline(
    const GraphicsPipelineConfig& config,
    VkPipelineLayout& pipelineLayout,
    VkPipeline& graphicsPipeline);
}
