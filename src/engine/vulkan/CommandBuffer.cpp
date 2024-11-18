#include "CommandBuffer.hpp"

#include "Swapchain.hpp"
#include "VkContext.hpp"
#include "math/definitions.hpp"
#include "rhi/vulkan_debug.hpp"

#include <cassert>
#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace Zeus
{
CommandBuffer::CommandBuffer(
    const CommandPool& commandPool,
    const char* name,
    VkCommandBufferLevel level)
    : m_commandPool{ &commandPool }
{
    m_handle = m_commandPool->AllocateBuffer(level);
    VkContext::SetDebugName(VK_OBJECT_TYPE_COMMAND_BUFFER, m_handle, name);
}

CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept
    : m_handle{ other.m_handle },
      m_commandPool{ other.m_commandPool }
{
    other.m_handle = VK_NULL_HANDLE;
    other.m_commandPool = nullptr;
}

CommandBuffer& CommandBuffer::operator=(CommandBuffer&& other)
{
    if (this != &other)
    {
        if (m_handle != VK_NULL_HANDLE)
        {
            vkFreeCommandBuffers(
                VkContext::GetLogicalDevice(),
                m_commandPool->GetHandle(),
                1,
                &m_handle);
        }

        m_handle = other.m_handle;
        m_commandPool = other.m_commandPool;

        other.m_handle = VK_NULL_HANDLE;
        other.m_commandPool = nullptr;
    }

    return *this;
}

void CommandBuffer::Begin(VkCommandBufferUsageFlags flags)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = flags;
    beginInfo.pInheritanceInfo = nullptr;

    VKCHECK(
        vkBeginCommandBuffer(m_handle, &beginInfo),
        "Failed to begin recording command buffer.");
}

void CommandBuffer::End()
{
    VKCHECK(vkEndCommandBuffer(m_handle), "Failed to record command buffer");
}

void CommandBuffer::Reset()
{
    VKCHECK(
        vkResetCommandBuffer(m_handle, 0),
        "Failed to reset command buffer");
}

// RHI_Queue* queue, const uint64_t swapchain_id
void CommandBuffer::Submit(
    VkQueue queue,
    VkFence fence,
    std::uint32_t waitSemaphoreInfoCount,
    const VkSemaphoreSubmitInfo* pWaitSemaphoreInfos,
    std::uint32_t commandBufferInfoCount,
    const VkCommandBufferSubmitInfo* pCommandBufferInfos,
    std::uint32_t signalSemaphoreInfoCount,
    const VkSemaphoreSubmitInfo* pSignalSemaphoreInfos)
{
    VkSubmitInfo2 submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;

    submitInfo.waitSemaphoreInfoCount = waitSemaphoreInfoCount;
    submitInfo.pWaitSemaphoreInfos = pWaitSemaphoreInfos;

    submitInfo.commandBufferInfoCount = commandBufferInfoCount;
    submitInfo.pCommandBufferInfos = pCommandBufferInfos;

    submitInfo.signalSemaphoreInfoCount = signalSemaphoreInfoCount;
    submitInfo.pSignalSemaphoreInfos = pSignalSemaphoreInfos;

    VkResult result{ vkQueueSubmit2(queue, 1, &submitInfo, fence) };

    VKCHECK(result, "Failed to submit command buffer.");
}

// push pipeline state
void CommandBuffer::BindPipeline(const Pipeline& pipeline)
{
    vkCmdBindPipeline(m_handle, pipeline.GetBindPoint(), pipeline.GetHandle());
}

void CommandBuffer::BeginRenderPass(
    VkRenderPass renderPass,
    const VkExtent2D& extent,
    VkFramebuffer framebuffer,
    const std::vector<VkClearValue>& clearValues,
    VkOffset2D offset,
    VkSubpassContents contents)
{
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = framebuffer;
    renderPassInfo.renderArea.offset = offset;
    renderPassInfo.renderArea.extent = extent;

    renderPassInfo.clearValueCount =
        static_cast<std::uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(m_handle, &renderPassInfo, contents);
}

void CommandBuffer::EndRenderPass()
{
    vkCmdEndRenderPass(m_handle);
}

void CommandBuffer::BeginRendering(
    const VkExtent2D& renderExtent,
    std::uint32_t colorAttachmentCount,
    const VkRenderingAttachmentInfo* pColorAttachments,
    const VkRenderingAttachmentInfo* pDepthAttachment,
    const VkRenderingAttachmentInfo* pStencilAttachment,
    VkRenderingFlags flags,
    std::uint32_t layerCount,
    std::uint32_t viewMask)
{
    VkRenderingInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.flags = flags;
    renderingInfo.renderArea = VkRect2D{ VkOffset2D{ 0, 0 }, renderExtent };
    renderingInfo.layerCount = layerCount;
    renderingInfo.viewMask = viewMask;
    renderingInfo.colorAttachmentCount = colorAttachmentCount;
    renderingInfo.pColorAttachments = pColorAttachments;
    renderingInfo.pDepthAttachment = pDepthAttachment;
    renderingInfo.pStencilAttachment = pStencilAttachment;

    vkCmdBeginRendering(m_handle, &renderingInfo);
}

void CommandBuffer::EndRendering()
{
    vkCmdEndRendering(m_handle);
}

// todo: combine clear methods to handle both types
void CommandBuffer::ClearColorImage(VkImage image, const Color& color)
{
    VkClearColorValue clearValue{ { color.r, color.g, color.b, color.a } };

    VkImageSubresourceRange subresourceRange{};
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

    vkCmdClearColorImage(
        m_handle,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        &clearValue,
        1,
        &subresourceRange);
}

void CommandBuffer::ClearDepthStencilImage(
    VkImage image,
    const VkClearDepthStencilValue& clearValue)
{
    VkImageSubresourceRange subresourceRange{};
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

    vkCmdClearDepthStencilImage(
        m_handle,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        &clearValue,
        1,
        &subresourceRange);
}

void CommandBuffer::Draw(
    std::uint32_t vertexCount,
    std::uint32_t firstVertex,
    std::uint32_t instanceCount,
    std::uint32_t firstInstance) const
{
    vkCmdDraw(m_handle, vertexCount, instanceCount, firstVertex, firstInstance);
}

void CommandBuffer::DrawIndexed(
    std::uint32_t indexCount,
    std::uint32_t firstIndex,
    std::int32_t vertexOffset,
    std::uint32_t instanceCount,
    std::uint32_t firstInstance) const
{
    vkCmdDrawIndexed(
        m_handle,
        indexCount,
        instanceCount,
        firstIndex,
        vertexOffset,
        firstInstance);
}

void CommandBuffer::Dispatch(
    std::uint32_t groupCountX,
    std::uint32_t groupCountY,
    std::uint32_t groupCountZ) const
{
    vkCmdDispatch(m_handle, groupCountX, groupCountY, groupCountZ);
}

//     const bool blit_mips);
void CommandBuffer::CopyBuffer(
    VkBuffer srcBuffer,
    VkBuffer dstBuffer,
    VkDeviceSize size,
    VkDeviceSize srcOffset,
    VkDeviceSize dstOffset) const
{
    VkBufferCopy2 bufferCopy{};
    bufferCopy.sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2;
    bufferCopy.srcOffset = srcOffset;
    bufferCopy.dstOffset = dstOffset;
    bufferCopy.size = size;

    VkCopyBufferInfo2 copyBufferInfo{};
    copyBufferInfo.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2;
    copyBufferInfo.pNext = nullptr;
    copyBufferInfo.srcBuffer = srcBuffer;
    copyBufferInfo.dstBuffer = dstBuffer;
    copyBufferInfo.regionCount = 1;
    copyBufferInfo.pRegions = &bufferCopy;

    vkCmdCopyBuffer2(m_handle, &copyBufferInfo);
}

void CommandBuffer::CopyImage(const Image& srcImage, const Image& dstImage)
    const
{
    VkImageCopy2 region{};
    region.sType = VK_STRUCTURE_TYPE_IMAGE_COPY_2;
    region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.srcSubresource.mipLevel = 0;
    region.srcSubresource.baseArrayLayer = 0;
    region.srcSubresource.layerCount = 1;
    region.srcOffset = {};
    region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.dstSubresource.mipLevel = 0;
    region.dstSubresource.baseArrayLayer = 0;
    region.dstSubresource.layerCount = 1;
    region.dstOffset = {};
    region.extent = srcImage.GetExtent();

    VkCopyImageInfo2 copyImageInfo{};
    copyImageInfo.sType = VK_STRUCTURE_TYPE_COPY_IMAGE_INFO_2;
    copyImageInfo.srcImage = srcImage.GetHandle();
    copyImageInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    copyImageInfo.dstImage = dstImage.GetHandle();
    copyImageInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    copyImageInfo.regionCount = 1;
    copyImageInfo.pRegions = &region;

    vkCmdCopyImage2(m_handle, &copyImageInfo);
}

//     const bool blitMips,
//     const float sourceScaling = 1.0f);
void CommandBuffer::BlitImage(const Image& srcImage, const Image& dstImage)
    const
{
    assert(srcImage.GetLayout() == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    assert(dstImage.GetLayout() == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    assert(
        srcImage.GetWidth() <= dstImage.GetWidth() &&
        srcImage.GetHeight() <= dstImage.GetHeight());

    VkImageBlit2 blitRegion{};
    blitRegion.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;

    blitRegion.srcSubresource.aspectMask = srcImage.GetAspectMask();
    blitRegion.srcSubresource.mipLevel = 0;
    blitRegion.srcSubresource.baseArrayLayer = 0;
    blitRegion.srcSubresource.layerCount = 1;

    blitRegion.srcOffsets[1].x = static_cast<std::int32_t>(srcImage.GetWidth());
    blitRegion.srcOffsets[1].y =
        static_cast<std::int32_t>(srcImage.GetHeight());
    blitRegion.srcOffsets[1].z = static_cast<std::int32_t>(srcImage.GetDepth());

    blitRegion.dstSubresource.aspectMask = dstImage.GetAspectMask();
    blitRegion.dstSubresource.mipLevel = 0;
    blitRegion.dstSubresource.baseArrayLayer = 0;
    blitRegion.dstSubresource.layerCount = 1;

    blitRegion.dstOffsets[1].x = static_cast<std::int32_t>(dstImage.GetWidth());
    blitRegion.dstOffsets[1].y =
        static_cast<std::int32_t>(dstImage.GetHeight());
    blitRegion.dstOffsets[1].z = static_cast<std::int32_t>(dstImage.GetDepth());

    VkBlitImageInfo2 blitImageInfo{};
    blitImageInfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
    blitImageInfo.srcImage = srcImage.GetHandle();
    blitImageInfo.srcImageLayout = srcImage.GetLayout();
    blitImageInfo.dstImage = dstImage.GetHandle();
    blitImageInfo.dstImageLayout = dstImage.GetLayout();

    blitImageInfo.regionCount = 1;
    blitImageInfo.pRegions = &blitRegion;

    // bool width_equal = source->GetWidth() == destination->GetWidth();
    // bool height_equal = source->GetHeight() == destination->GetHeight();
    // Filter filter = widthEqual && heightEqual ? Nearest : Linear;
    blitImageInfo.filter = VK_FILTER_LINEAR;

    vkCmdBlitImage2(m_handle, &blitImageInfo);
}

void CommandBuffer::BlitImage(const Image& srcImage, const Swapchain& swapchain)
    const
{
    // assert(srcImage.GetLayout() == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    assert(swapchain.GetLayout() == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkImageBlit2 blitRegion{};
    blitRegion.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;

    blitRegion.srcSubresource.aspectMask = srcImage.GetAspectMask();
    blitRegion.srcSubresource.mipLevel = 0;
    blitRegion.srcSubresource.baseArrayLayer = 0;
    blitRegion.srcSubresource.layerCount = 1;

    blitRegion.srcOffsets[1].x = static_cast<std::int32_t>(srcImage.GetWidth());
    blitRegion.srcOffsets[1].y =
        static_cast<std::int32_t>(srcImage.GetHeight());
    blitRegion.srcOffsets[1].z = static_cast<std::int32_t>(srcImage.GetDepth());

    blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blitRegion.dstSubresource.mipLevel = 0;
    blitRegion.dstSubresource.baseArrayLayer = 0;
    blitRegion.dstSubresource.layerCount = 1;

    blitRegion.dstOffsets[1].x =
        static_cast<std::int32_t>(swapchain.GetWidth());
    blitRegion.dstOffsets[1].y =
        static_cast<std::int32_t>(swapchain.GetHeight());
    blitRegion.dstOffsets[1].z = 1;

    VkBlitImageInfo2 blitImageInfo{};
    blitImageInfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
    blitImageInfo.srcImage = srcImage.GetHandle();
    blitImageInfo.srcImageLayout =
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL; // srcImage.GetLayout();
    blitImageInfo.dstImage = swapchain.GetImage();
    blitImageInfo.dstImageLayout =
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL; // swapchain.GetLayout();

    blitImageInfo.regionCount = 1;
    blitImageInfo.pRegions = &blitRegion;
    blitImageInfo.filter = VK_FILTER_LINEAR;

    vkCmdBlitImage2(m_handle, &blitImageInfo);
}

void CommandBuffer::BindDescriptorSets(
    VkDescriptorSet descriptorSet,
    const Pipeline& pipeline) const
{
    vkCmdBindDescriptorSets(
        m_handle,
        pipeline.GetBindPoint(),
        pipeline.GetLayout(),
        0,
        1,
        &descriptorSet,
        0,
        nullptr);
}

void CommandBuffer::BindVertexBuffers(VkBuffer buffer) const
{
    VkBuffer vertexBuffers[] = { buffer };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(m_handle, 0, 1, vertexBuffers, offsets);
}

void CommandBuffer::BindIndexBuffer(VkBuffer buffer) const
{
    bool isInt16 = false; // buffer->GetStride() == sizeof(uint16_t);

    vkCmdBindIndexBuffer(
        m_handle,
        buffer,
        0,
        isInt16 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
}

void CommandBuffer::TransitionImageLayout(
    VkImage image,
    VkFormat format,
    VkImageLayout currentLayout,
    VkImageLayout newLayout) const
{
    VkImageMemoryBarrier2 imageMemoryBarrier{};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    imageMemoryBarrier.pNext = nullptr;

    // if (currentLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
    //     newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    // {
    //     imageMemoryBarrier.srcStageMask =
    //         VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT; //
    //         VK_PIPELINE_STAGE_2_NONE;
    //     imageMemoryBarrier.srcAccessMask = VK_ACCESS_2_NONE;
    //     imageMemoryBarrier.dstStageMask =
    //     VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    //     imageMemoryBarrier.dstAccessMask =
    //     VK_ACCESS_2_TRANSFER_WRITE_BIT;
    // }
    // else if (
    //     currentLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
    //     (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL ||
    //      newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL))
    // {
    //     imageMemoryBarrier.srcStageMask =
    //         VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT; //
    //         VK_PIPELINE_STAGE_2_NONE;
    //     imageMemoryBarrier.srcAccessMask = VK_ACCESS_2_NONE;
    //     imageMemoryBarrier.dstStageMask =
    //         VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT;
    //     imageMemoryBarrier.dstAccessMask =
    //         VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
    //         VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    // }
    // else if (
    //     currentLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
    //     newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    // {
    //     imageMemoryBarrier.srcStageMask =
    //     VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    //     imageMemoryBarrier.srcAccessMask =
    //     VK_ACCESS_2_TRANSFER_WRITE_BIT; imageMemoryBarrier.dstStageMask =
    //         VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
    //     imageMemoryBarrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
    // }
    // else
    {
        // error("Unsupported image layout transition.");
        imageMemoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
        imageMemoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        imageMemoryBarrier.dstAccessMask =
            VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
        // assert(false);
    }

    imageMemoryBarrier.oldLayout = currentLayout;
    imageMemoryBarrier.newLayout = newLayout;

    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    imageMemoryBarrier.image = image;

    VkImageAspectFlags aspectMask;
    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL ||
        newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        if (vkuFormatIsDepthAndStencil(format))
        {
            aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    }
    else
    {
        aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    imageMemoryBarrier.subresourceRange.aspectMask = aspectMask;
    imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
    imageMemoryBarrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

    VkDependencyInfo dependencyInfo{};
    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependencyInfo.pNext = nullptr;

    dependencyInfo.imageMemoryBarrierCount = 1;
    dependencyInfo.pImageMemoryBarriers = &imageMemoryBarrier;

    vkCmdPipelineBarrier2(m_handle, &dependencyInfo);
}

void CommandBuffer::SetViewport(const VkViewport& viewport) const
{
    assert(viewport.width != 0);
    assert(viewport.height != 0);

    vkCmdSetViewport(m_handle, 0, 1, &viewport);
}

void CommandBuffer::SetScissor(const VkRect2D& scissor) const
{
    assert(scissor.extent.width != 0);
    assert(scissor.extent.height != 0);

    vkCmdSetScissor(m_handle, 0, 1, &scissor);
}

void CommandBuffer::SetCullMode(VkCullModeFlags cullMode) const
{
    vkCmdSetCullMode(m_handle, cullMode);
}

void CommandBuffer::BeginDebugLabel(const char* pLabelName, const Color& color)
    const
{
    cmdBeginDebugUtilsLabelEXT(m_handle, pLabelName, color);
}

void CommandBuffer::InsertDebugLabel(const char* pLabelName, const Color& color)
    const
{
    cmdInsertDebugUtilsLabelEXT(m_handle, pLabelName, color);
}

void CommandBuffer::EndDebugLabel() const
{
    cmdEndDebugUtilsLabelEXT(m_handle);
}

VkCommandBuffer CommandBuffer::GetHandle() const
{
    return m_handle;
}
}
