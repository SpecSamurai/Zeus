#pragma once

#include <vulkan/vulkan.h>

namespace Zeus
{
struct RenderPassConfig
{
    VkFormat colorAttachmentFormat;
    VkFormat depthAttachmentFormat;
    VkSampleCountFlagBits msaaSamples;
};

VkResult createVkRenderPass(
    const VkDevice& device,
    const RenderPassConfig& config,
    VkRenderPass& renderPass);
}
