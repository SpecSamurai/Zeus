#pragma once

#include <vulkan/vulkan_core.h>

namespace Zeus
{
bool createVkRenderPass(
    VkDevice device,
    VkFormat swapChainImageFormat,
    VkSampleCountFlagBits msaaSamples,
    VkFormat depthFormat,
    VkRenderPass& renderPass);
}
