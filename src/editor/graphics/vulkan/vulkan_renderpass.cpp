#include "vulkan_renderpass.hpp"

#include "vulkan_utils.hpp"

#include <core/logger.hpp>

#include <vulkan/vulkan_core.h>

#include <array>
#include <cstdint>

namespace Zeus
{
bool createVkRenderPass(
    const VkDevice& device,
    const RenderPassConfig& config,
    VkRenderPass& renderPass)
{
    VkAttachmentDescription colorAttachmentDescription{};
    colorAttachmentDescription.format = config.colorAttachmentFormat;
    colorAttachmentDescription.samples = config.msaaSamples;
    colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentDescription.stencilStoreOp =
        VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentDescription.finalLayout =
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // VkAttachmentDescription depthAttachment{};
    // depthAttachment.format = depthFormat;
    // depthAttachment.samples = msaaSamples;
    // depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    // depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    // depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    // depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    // depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    // depthAttachment.finalLayout =
    //     VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    //
    // VkAttachmentDescription colorAttachmentResolve{};
    // colorAttachmentResolve.format = swapChainImageFormat;
    // colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
    // colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    // colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    // colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    // colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    // colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    // colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentReference{};
    // Index of VkRenderPassCreateInfo's attachment_descriptions array
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // VkAttachmentReference depthAttachmentRef{};
    // depthAttachmentRef.attachment = 1;
    // depthAttachmentRef.layout =
    //     VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    //
    // VkAttachmentReference colorAttachmentResolveRef{};
    // colorAttachmentResolveRef.attachment = 2;
    // colorAttachmentResolveRef.layout =
    // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentReference;
    // subpass.pDepthStencilAttachment = &depthAttachmentRef;
    // subpass.pResolveAttachments = &colorAttachmentResolveRef;

    VkSubpassDependency subpassDependency{};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    // | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependency.srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    // | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependency.dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.srcAccessMask = 0;
    // | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependency.dependencyFlags = 0;

    std::array<VkAttachmentDescription, 1> attachments = {
        colorAttachmentDescription,
        // depthAttachment,
        // colorAttachmentResolve,
    };

    VkRenderPassCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = static_cast<std::uint32_t>(attachments.size());
    // The VkAttachmentReference objects reference attachments using the
    // indices of this array.
    createInfo.pAttachments = attachments.data();
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpass;
    createInfo.dependencyCount = 1;
    createInfo.pDependencies = &subpassDependency;

    VkResult result{
        vkCreateRenderPass(device, &createInfo, nullptr, &renderPass)
    };

    if (result != VK_SUCCESS)
    {
        error("Failed to create render pass. {}", vkResultToString(result));
    }

    return result == VK_SUCCESS;
}
}
