#include "vulkan_renderpass.hpp"

#include "vulkan_utils.hpp"

#include <core/logger.hpp>

#include <vulkan/vulkan_core.h>

#include <array>

namespace Zeus
{
bool createVkRenderPass(
    VkDevice device,
    VkFormat swapChainImageFormat,
    VkSampleCountFlagBits msaaSamples,
    VkFormat depthFormat,
    VkRenderPass& renderPass)
{
    VkAttachmentDescription colorAttachment{};
    // 0,                     // VkAttachmentDescriptionFlags   flags
    // The format of the color attachment should match the format of the
    // swap chain images, and we're not doing anything with multisampling
    // yet, so we'll stick to 1 sample.
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = msaaSamples;
    // The loadOp and storeOp determine what to do with the data in the
    // attachment before rendering and after rendering. In our case we're
    // going to use the clear operation to clear the framebuffer to black
    // before drawing a new frame. We're interested in seeing the rendered
    // triangle on the screen, so we're going with the store operation here.
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    // he loadOp and storeOp apply to color and depth data, and
    // stencilLoadOp / stencilStoreOp apply to stencil data. Our application
    // won't do anything with the stencil buffer, so the results of loading
    // and storing are irrelevant.
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    // Textures and framebuffers in Vulkan are represented by VkImage
    // objects with a certain pixel format, however the layout of the pixels
    // in memory can change based on what you're trying to do with an image.
    // images need to be transitioned to specific layouts that are suitable
    // for the operation that they're going to be involved in next. The
    // initialLayout specifies which layout the image will have before the
    // render pass begins. The finalLayout specifies the layout to
    // automatically transition to when the render pass finishes. Using
    // VK_IMAGE_LAYOUT_UNDEFINED for initialLayout means that we don't care
    // what previous layout the image was in. The caveat of this special
    // value is that the contents of the image are not guaranteed to be
    // preserved, but that doesn't matter since we're going to clear it
    // anyway. We want the image to be ready for presentation using the swap
    // chain after rendering, which is why we use
    // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR as finalLayout.
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    // That's because multisampled images cannot be presented directly. We
    // first need to resolve them to a regular image. This requirement does
    // not apply to the depth buffer, since it won't be presented at any
    // point. Therefore we will have to add only one new attachment for
    // color which is a so-called resolve attachment:
    colorAttachment.finalLayout =
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = depthFormat;
    depthAttachment.samples = msaaSamples;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout =
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachmentResolve{};
    colorAttachmentResolve.format = swapChainImageFormat;
    colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // Every subpass references one or more of the attachments that we've
    // described using the structure in the previous sections. The
    // attachment parameter specifies which attachment to reference by its
    // index in the attachment descriptions array. Our array consists of a
    // single VkAttachmentDescription, so its index is 0. The layout
    // specifies which layout we would like the attachment to have during a
    // subpass that uses this reference. Vulkan will automatically
    // transition the attachment to this layout when the subpass is started.
    // We intend to use the attachment to function as a color buffer and the
    // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL layout will give us the best
    // performance, as its name implies.
    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout =
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentResolveRef{};
    colorAttachmentResolveRef.attachment = 2;
    colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // A single render pass can consist of multiple subpasses. Subpasses are
    // subsequent rendering operations that depend on the contents of
    // framebuffers in previous passes, for example a sequence of
    // post-processing effects that are applied one after another. If you
    // group these rendering operations into one render pass, then Vulkan is
    // able to reorder the operations and conserve memory bandwidth for
    // possibly better performance. For our very first triangle, however,
    // we'll stick to a single subpass.
    VkSubpassDescription subpass{};
    // Vulkan may also support compute subpasses in the future, so we have
    // to be explicit about this being a graphics subpass.
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;
    subpass.pResolveAttachments = &colorAttachmentResolveRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                              VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                              VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                               VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 3> attachments = {
        colorAttachment,
        depthAttachment,
        colorAttachmentResolve,
    };

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    // renderPassInfo.attachmentCount = 1;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    // The VkAttachmentReference objects reference attachments using the
    // indices of this array.
    // renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VkResult result{
        vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass)
    };

    if (result != VK_SUCCESS)
    {
        error("Failed to create render pass. {}", vkResultToString(result));
    }

    return result == VK_SUCCESS;
}
}
