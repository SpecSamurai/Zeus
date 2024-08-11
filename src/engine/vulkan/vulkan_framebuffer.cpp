#include "vulkan_framebuffer.hpp"

#include "vulkan/vulkan_utils.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
VkResult createVkFramebuffer(
    VkFramebuffer& framebuffer,
    const VkDevice& device,
    const std::vector<VkImageView>& attachments,
    const VkRenderPass& renderPass,
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

    VkResult result{
        vkCreateFramebuffer(device, &createInfo, nullptr, &framebuffer)
    };

    VKCHECK(result, "Failed to create Framebuffer.");

    return result;
}
}
