#pragma once

#include <vulkan/vulkan_core.h>

namespace Zeus
{
struct RenderPassConfig
{
    VkFormat colorAttachmentFormat;
    VkFormat depthAttachmentFormat;
    VkSampleCountFlagBits msaaSamples;
};

bool createVkRenderPass(
    const VkDevice& device,
    const RenderPassConfig& config,
    VkRenderPass& renderPass);
}
