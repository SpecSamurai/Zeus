#include "vulkan_framebuffer.hpp"

#include "vulkan_debug.hpp"
#include "vulkan_memory.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
VkResult createVkFramebuffer(
    VkFramebuffer* framebuffer,
    VkDevice device,
    const std::vector<VkImageView>& attachments,
    VkRenderPass renderPass,
    const VkExtent2D& extent,
    std::uint32_t layers)
{
    VkFramebufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.renderPass = renderPass;
    createInfo.attachmentCount = static_cast<std::uint32_t>(attachments.size());
    createInfo.pAttachments = attachments.data();
    createInfo.width = extent.width;
    createInfo.height = extent.height;
    createInfo.layers = layers;

    VkResult result{ vkCreateFramebuffer(
        device,
        &createInfo,
        allocationCallbacks.get(),
        framebuffer) };

    VKCHECK(result, "Failed to create Framebuffer.");

    return result;
}
}
