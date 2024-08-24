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

void cmdBeginRendering(
    VkCommandBuffer commandBuffer,
    VkExtent2D renderExtent, // VkRect2D renderArea,
    uint32_t colorAttachmentCount,
    const VkRenderingAttachmentInfo* pColorAttachments,
    const VkRenderingAttachmentInfo* pDepthAttachment,
    const VkRenderingAttachmentInfo* pStencilAttachment,
    VkRenderingFlags flags,
    uint32_t layerCount,
    uint32_t viewMask)
{
    VkRenderingInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.flags = flags;
    renderingInfo.renderArea =
        VkRect2D{ VkOffset2D{ 0, 0 }, renderExtent }; // renderArea;
    renderingInfo.layerCount = layerCount;
    renderingInfo.viewMask = viewMask;
    renderingInfo.colorAttachmentCount = colorAttachmentCount;
    renderingInfo.pColorAttachments = pColorAttachments;
    renderingInfo.pDepthAttachment = pDepthAttachment;
    renderingInfo.pStencilAttachment = pStencilAttachment;

    vkCmdBeginRendering(commandBuffer, &renderingInfo);
}
}
