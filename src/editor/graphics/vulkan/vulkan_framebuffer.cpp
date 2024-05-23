#include "vulkan_framebuffer.hpp"

#include "vulkan_utils.hpp"

#include <core/logger.hpp>

#include <vulkan/vulkan_core.h>

#include <array>

namespace Zeus
{
bool createVkFramebuffer(
    const VkDevice& device,
    const std::array<VkImageView, 3>& attachments,
    const VkRenderPass& renderPass,
    const VkExtent2D& swapChainExtent,
    VkFramebuffer& framebuffer)
{
    VkFramebufferCreateInfo framebufferInfo{
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = renderPass,
        .attachmentCount = static_cast<std::uint32_t>(attachments.size()),
        .pAttachments = attachments.data(),
        .width = swapChainExtent.width,
        .height = swapChainExtent.height,
        .layers = 1,
    };

    VkResult result{
        vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer)
    };

    if (result != VK_SUCCESS)
    {
        error("Failed to create framebuffer. {}", vkResultToString(result));
    }

    return result == VK_SUCCESS;
}
}
