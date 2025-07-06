#include "CommandBuffer.hpp"

#include "Buffer.hpp"
#include "VkContext.hpp"
#include "math/definitions.hpp"
#include "vulkan/vulkan_debug.hpp"

#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cstdint>
#include <span>
#include <string_view>

namespace Zeus
{
CommandBuffer::CommandBuffer(
    std::string_view name,
    const CommandPool& commandPool,
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
                VkContext::LogicalDevice(),
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

void CommandBuffer::EndRenderPass() const
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
    std::uint32_t viewMask) const
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

void CommandBuffer::EndRendering() const
{
    vkCmdEndRendering(m_handle);
}

void CommandBuffer::ClearColorImage(VkImage image, const Math::Color& color)
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

void CommandBuffer::CopyBuffer(
    VkBuffer srcBuffer,
    VkBuffer dstBuffer,
    std::span<VkBufferCopy2> regions) const
{
    VkCopyBufferInfo2 copyBufferInfo{};
    copyBufferInfo.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2;
    copyBufferInfo.pNext = nullptr;
    copyBufferInfo.srcBuffer = srcBuffer;
    copyBufferInfo.dstBuffer = dstBuffer;
    copyBufferInfo.regionCount = static_cast<std::uint32_t>(regions.size());
    copyBufferInfo.pRegions = regions.data();

    vkCmdCopyBuffer2(m_handle, &copyBufferInfo);
}

void CommandBuffer::CopyBufferToImage(
    VkBuffer srcBuffer,
    VkImage dstImage,
    std::span<VkBufferImageCopy2> regions) const
{
    VkCopyBufferToImageInfo2 info{};
    info.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2;
    info.pNext = nullptr;
    info.srcBuffer = srcBuffer;
    info.dstImage = dstImage;
    info.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    info.regionCount = static_cast<std::uint32_t>(regions.size());
    info.pRegions = regions.data();

    vkCmdCopyBufferToImage2(m_handle, &info);
}

void CommandBuffer::CopyImage(
    VkImage srcImage,
    VkImage dstImage,
    std::span<VkImageCopy2> regions) const
{
    VkCopyImageInfo2 info{};
    info.sType = VK_STRUCTURE_TYPE_COPY_IMAGE_INFO_2;
    info.srcImage = srcImage;
    info.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    info.dstImage = dstImage;
    info.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    info.regionCount = static_cast<std::uint32_t>(regions.size());
    info.pRegions = regions.data();

    vkCmdCopyImage2(m_handle, &info);
}

void CommandBuffer::BlitImage(
    VkImage srcImage,
    VkImage dstImage,
    std::span<VkImageBlit2> regions,
    VkFilter filter) const
{
    VkBlitImageInfo2 info{};
    info.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
    info.srcImage = srcImage;
    info.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    info.dstImage = dstImage;
    info.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    info.regionCount = static_cast<std::uint32_t>(regions.size());
    info.pRegions = regions.data();
    info.filter = filter;

    vkCmdBlitImage2(m_handle, &info);
}

void CommandBuffer::BindDescriptorSets(
    VkDescriptorSet descriptorSet,
    const Pipeline& pipeline,
    std::uint32_t firstSet,
    std::span<std::uint32_t> dynamicOffsets) const
{
    vkCmdBindDescriptorSets(
        m_handle,
        pipeline.GetBindPoint(),
        pipeline.GetLayout(),
        firstSet,
        1,
        &descriptorSet,
        static_cast<std::uint32_t>(dynamicOffsets.size()),
        dynamicOffsets.data());
}

void CommandBuffer::BindVertexBuffers(const Buffer& buffer) const
{
    assert(buffer.GetUsage() & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    VkBuffer vertexBuffers[] = { buffer.GetHandle() };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(m_handle, 0, 1, vertexBuffers, offsets);
}

void CommandBuffer::BindIndexBuffer(const Buffer& buffer) const
{
    assert(buffer.GetUsage() & VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    bool isInt16 = false; // buffer->GetStride() == sizeof(uint16_t);

    vkCmdBindIndexBuffer(
        m_handle,
        buffer.GetHandle(),
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

void CommandBuffer::TransitionImageLayout(
    VkImage image,
    VkFormat format,
    VkPipelineStageFlags2 srcStageMask,
    VkAccessFlags2 srcAccessMask,
    VkPipelineStageFlags2 dstStageMask,
    VkAccessFlags2 dstAccessMask,
    VkImageLayout currentLayout,
    VkImageLayout newLayout) const
{
    VkImageMemoryBarrier2 imageMemoryBarrier{};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    imageMemoryBarrier.pNext = nullptr;

    imageMemoryBarrier.srcStageMask = srcStageMask;
    imageMemoryBarrier.srcAccessMask = srcAccessMask;
    imageMemoryBarrier.dstStageMask = dstStageMask;
    imageMemoryBarrier.dstAccessMask = dstAccessMask;

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

void CommandBuffer::BeginDebugLabel(
    const char* pLabelName,
    const Math::Color& color) const
{
    cmdBeginDebugUtilsLabelEXT(m_handle, pLabelName, color);
}

void CommandBuffer::InsertDebugLabel(
    const char* pLabelName,
    const Math::Color& color) const
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
