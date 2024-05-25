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
    // Array of image views representing all attachments used in a framebuffer
    // and render pass. Each element in this array (each image view) corresponds
    // to each attachment in a render pass.
    // each pAttachments array’s element corresponds directly to an attachment
    // in a render pass description structure.
    // But we may use a framebuffer not only with the specified render pass but
    // also with all render passes that are compatible with the one specified.
    // Compatible render passes, in general, must have the same number of
    // attachments and corresponding attachments must have the same format and
    // number of samples. But image layouts (initial, final, and for each
    // subpass) may differ and doesn’t involve render pass compatibility.
    VkFramebufferCreateInfo framebufferInfo{
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = renderPass,
        .attachmentCount = static_cast<std::uint32_t>(attachments.size()),
        .pAttachments = attachments.data(),
        .width = swapChainExtent.width,
        .height = swapChainExtent.height,
        .layers = 1,
    };

    // The framebuffer specifies what images are used as attachments on which
    // the render pass operates. We can say that it translates image (image
    // view) into a given attachment.
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
