#include "vulkan_dynamic_rendering.hpp"

#include <vulkan/vulkan.h>

namespace Zeus
{
VkRenderingAttachmentInfo createDepthAttachmentInfo(
    VkImageView imageView,
    VkImageLayout imageLayout)
{
    VkRenderingAttachmentInfo renderingAttachmentInfo{};
    renderingAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    renderingAttachmentInfo.imageView = imageView;
    renderingAttachmentInfo.imageLayout = imageLayout;
    renderingAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    renderingAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    // Reverse depth value: 0.f
    // Normal depth: 1.f
    renderingAttachmentInfo.clearValue.depthStencil.depth = 0.f;

    return renderingAttachmentInfo;
}

VkRenderingAttachmentInfo createColorAttachmentInfo(
    VkImageView imageView,
    VkImageLayout imageLayout,
    const VkClearValue* clearValue)
{
    VkRenderingAttachmentInfo renderingAttachmentInfo{};
    renderingAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    renderingAttachmentInfo.imageView = imageView;
    renderingAttachmentInfo.imageLayout = imageLayout;
    renderingAttachmentInfo.resolveMode = VK_RESOLVE_MODE_NONE;
    renderingAttachmentInfo.resolveImageView = VK_NULL_HANDLE;
    renderingAttachmentInfo.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    renderingAttachmentInfo.loadOp =
        clearValue ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    renderingAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    if (clearValue)
    {
        renderingAttachmentInfo.clearValue = *clearValue;
    }

    return renderingAttachmentInfo;
}
}
