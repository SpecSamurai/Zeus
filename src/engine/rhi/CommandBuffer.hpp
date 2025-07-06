#pragma once

#include "CommandPool.hpp"
#include "Pipeline.hpp"
#include "math/definitions.hpp"

#include <vulkan/utility/vk_format_utils.h>
#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cstdint>
#include <span>
#include <vector>

namespace Zeus
{
class Swapchain;
class Buffer;

class CommandBuffer
{
public:
    CommandBuffer() = default;
    CommandBuffer(
        std::string_view name,
        const CommandPool& commandPool,
        VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer& operator=(const CommandBuffer&) = delete;

    CommandBuffer(CommandBuffer&& other) noexcept;
    CommandBuffer& operator=(CommandBuffer&& other);

    void Begin(VkCommandBufferUsageFlags flags = {});
    void End();
    void Reset();

    void BindPipeline(const Pipeline& pipeline);

    void BeginRenderPass(
        VkRenderPass renderPass,
        const VkExtent2D& extent,
        VkFramebuffer framebuffer,
        const std::vector<VkClearValue>& clearValues,
        VkOffset2D offset = { .x = 0, .y = 0 },
        VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE);

    void EndRenderPass() const;

    void BeginRendering(
        const VkExtent2D& renderExtent,
        std::uint32_t colorAttachmentCount,
        const VkRenderingAttachmentInfo* pColorAttachments,
        const VkRenderingAttachmentInfo* pDepthAttachment = nullptr,
        const VkRenderingAttachmentInfo* pStencilAttachment = nullptr,
        VkRenderingFlags flags = 0,
        std::uint32_t layerCount = 1,
        std::uint32_t viewMask = 0) const;

    void EndRendering() const;

    void ClearColorImage(VkImage image, const Math::Color& color);

    void ClearDepthStencilImage(
        VkImage image,
        const VkClearDepthStencilValue& clearValue);

    void Draw(
        std::uint32_t vertexCount,
        std::uint32_t firstVertex,
        std::uint32_t instanceCount = 1,
        std::uint32_t firstInstance = 0) const;

    void DrawIndexed(
        std::uint32_t indexCount,
        std::uint32_t firstIndex = 0,
        std::int32_t vertexOffset = 0,
        std::uint32_t instanceCount = 1,
        std::uint32_t firstInstance = 0) const;

    void Dispatch(
        std::uint32_t groupCountX,
        std::uint32_t groupCountY,
        std::uint32_t groupCountZ) const;

    void CopyBuffer(
        VkBuffer srcBuffer,
        VkBuffer dstBuffer,
        std::span<VkBufferCopy2> regions) const;

    void CopyBufferToImage(
        VkBuffer srcBuffer,
        VkImage dstImage,
        std::span<VkBufferImageCopy2> regions) const;

    void CopyImage(
        VkImage srcImage,
        VkImage dstImage,
        std::span<VkImageCopy2> regions) const;

    void BlitImage(
        VkImage srcImage,
        VkImage dstImage,
        std::span<VkImageBlit2> regions,
        VkFilter filter = VK_FILTER_LINEAR) const;

    void BindDescriptorSets(
        VkDescriptorSet descriptorSet,
        const Pipeline& pipeline,
        std::uint32_t firstSet = 0,
        std::span<std::uint32_t> dynamicOffsets = {}) const;

    void BindVertexBuffers(const Buffer& buffer) const;
    void BindIndexBuffer(const Buffer& buffer) const;

    template <typename T>
    void PushConstants(
        VkPipelineLayout layout,
        VkShaderStageFlags shaderStage,
        const std::uint32_t offset,
        const T& data) const
    {
        vkCmdPushConstants(
            m_handle,
            layout,
            shaderStage,
            offset,
            sizeof(T),
            &data);
    }

    void TransitionImageLayout(
        VkImage image,
        VkFormat format,
        VkImageLayout currentLayout,
        VkImageLayout newLayout) const;

    void TransitionImageLayout(
        VkImage image,
        VkFormat format,
        VkPipelineStageFlags2 srcStageMask,
        VkAccessFlags2 srcAccessMask,
        VkPipelineStageFlags2 dstStageMask,
        VkAccessFlags2 dstAccessMask,
        VkImageLayout currentLayout,
        VkImageLayout newLayout) const;

    void SetViewport(const VkViewport& viewport) const;
    void SetScissor(const VkRect2D& scissor) const;
    void SetCullMode(VkCullModeFlags cullMode) const;

    void BeginDebugLabel(const char* pLabelName, const Math::Color& color)
        const;
    void InsertDebugLabel(const char* pLabelName, const Math::Color& color)
        const;
    void EndDebugLabel() const;

    VkCommandBuffer GetHandle() const;

private:
    VkCommandBuffer m_handle{ VK_NULL_HANDLE };
    const CommandPool* m_commandPool{ nullptr };

    // semaprhors/fences?
    // state?
    // std::mutex m_mutex_reset?;
};
}
