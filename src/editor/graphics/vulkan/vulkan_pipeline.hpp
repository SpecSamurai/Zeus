#pragma once

#include <vulkan/vulkan_core.h>

namespace Zeus
{
bool CreateGraphicsPipeline(
    const VkDevice& device,
    VkRenderPass& renderPass,
    VkDescriptorSetLayout& descriptorSetLayout,
    VkPipelineLayout& pipelineLayout,
    VkPipeline& graphicsPipeline);
}
