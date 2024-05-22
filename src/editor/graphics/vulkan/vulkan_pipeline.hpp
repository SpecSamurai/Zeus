#pragma once

#include <vulkan/vulkan_core.h>

#include <array>

namespace Zeus
{
struct GraphicsPipelineConfig
{
    const VkDevice& device;
    const VkRenderPass& renderPass;
    const VkDescriptorSetLayout& descriptorSetLayout;
    const VkSampleCountFlagBits msaaSamples;
    const VkVertexInputBindingDescription& bindingDescription;
    const std::array<VkVertexInputAttributeDescription, 3>&
        attributeDescriptions;
};

bool createGraphicsVkPipeline(
    const GraphicsPipelineConfig& config,
    VkPipelineLayout& pipelineLayout,
    VkPipeline& graphicsPipeline);
}
