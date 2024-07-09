#include "vulkan_framebuffer.hpp"

#include "vulkan_utils.hpp"

#include <core/logger.hpp>

#include <vulkan/vulkan_core.h>

#include <array>

namespace Zeus
{
// The framebuffer specifies what images are used as attachments on which
// the render pass operates.
// We can say that it translates image (image view) into a given attachment.

// Array of image views representing all attachments used in a framebuffer
// and render pass. Each element in this array (each image view) corresponds
// to each attachment in a render pass.
// Each pAttachments array’s element corresponds directly to an attachment
// in a render pass description structure.

// We may use a framebuffer not only with the specified render pass but
// also with all render passes that are compatible with the one specified.
// Compatible render passes, in general, must have the same number of
// attachments and corresponding attachments must have the same format and
// number of samples. But image layouts (initial, final, and for each
// subpass) may differ and doesn’t involve render pass compatibility.
bool createVkFramebuffer(
    const VkDevice& device,
    const std::array<VkImageView, 1>& attachments,
    const VkRenderPass& renderPass,
    const VkExtent2D& swapChainExtent,
    VkFramebuffer& framebuffer)
{
    VkFramebufferCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = VK_NULL_HANDLE,
        .flags = 0,
        .renderPass = renderPass,
        .attachmentCount = static_cast<std::uint32_t>(attachments.size()),
        .pAttachments = attachments.data(),
        .width = swapChainExtent.width,
        .height = swapChainExtent.height,
        .layers = 1,
    };

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
