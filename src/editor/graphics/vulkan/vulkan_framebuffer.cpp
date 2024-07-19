#include "vulkan_framebuffer.hpp"

#include "vulkan_utils.hpp"

#include <core/logger.hpp>

#include <vulkan/vulkan_core.h>

#include <vector>

namespace Zeus
{
bool createVkFramebuffer(
    const VkDevice& device,
    const std::vector<VkImageView>& attachments,
    const VkRenderPass& renderPass,
    const VkExtent2D& swapChainExtent,
    VkFramebuffer& framebuffer)
{
    VkFramebufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.renderPass = renderPass;
    createInfo.attachmentCount = static_cast<std::uint32_t>(attachments.size());
    createInfo.pAttachments = attachments.data();
    createInfo.width = swapChainExtent.width;
    createInfo.height = swapChainExtent.height;
    createInfo.layers = 1;

    VkResult result{
        vkCreateFramebuffer(device, &createInfo, nullptr, &framebuffer)
    };

    if (result != VK_SUCCESS)
    {
        error("Failed to create framebuffer. {}", vkResultToString(result));
    }

    return result == VK_SUCCESS;
}
}
