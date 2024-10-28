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
    VkDevice device,
    const RenderPassConfig& config,
    VkRenderPass* renderPass);
}
