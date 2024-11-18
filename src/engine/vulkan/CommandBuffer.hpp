#pragma once

#include "CommandPool.hpp"
#include "Image.hpp"
#include "Pipeline.hpp"
#include "math/definitions.hpp"

#include <vulkan/utility/vk_format_utils.h>
#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cstdint>

namespace Zeus
{
class Swapchain;

class CommandBuffer
{
public:
    CommandBuffer() = default;
    CommandBuffer(
        const CommandPool& commandPool,
        const char* name = nullptr,
        VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer& operator=(const CommandBuffer&) = delete;

    CommandBuffer(CommandBuffer&& other) noexcept;
    CommandBuffer& operator=(CommandBuffer&& other);

    void Begin(VkCommandBufferUsageFlags flags = {});
    void End();
    void Reset();
    // RHI_Queue* queue, const uint64_t swapchain_id
    void Submit(
        VkQueue queue,
        VkFence fence,
        std::uint32_t waitSemaphoreInfoCount,
        const VkSemaphoreSubmitInfo* pWaitSemaphoreInfos,
        std::uint32_t commandBufferInfoCount,
        const VkCommandBufferSubmitInfo* pCommandBufferInfos,
        std::uint32_t signalSemaphoreInfoCount,
        const VkSemaphoreSubmitInfo* pSignalSemaphoreInfos);

    void BindPipeline(const Pipeline& pipeline);

    void BeginRenderPass(
        VkRenderPass renderPass,
        const VkExtent2D& extent,
        VkFramebuffer framebuffer,
        const std::vector<VkClearValue>& clearValues,
        VkOffset2D offset = { .x = 0, .y = 0 },
        VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE);

    void EndRenderPass();

    void BeginRendering(
        const VkExtent2D& renderExtent,
        std::uint32_t colorAttachmentCount,
        const VkRenderingAttachmentInfo* pColorAttachments,
        const VkRenderingAttachmentInfo* pDepthAttachment = nullptr,
        const VkRenderingAttachmentInfo* pStencilAttachment = nullptr,
        VkRenderingFlags flags = 0,
        std::uint32_t layerCount = 1,
        std::uint32_t viewMask = 0);

    void EndRendering();

    void ClearColorImage(VkImage image, const Color& color);

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

    //     const bool blit_mips);
    void CopyBuffer(
        VkBuffer srcBuffer,
        VkBuffer dstBuffer,
        VkDeviceSize size,
        VkDeviceSize srcOffset = 0,
        VkDeviceSize dstOffset = 0) const;

    void CopyImage(const Image& srcImage, const Image& dstImage) const;

    void BlitImage(const Image& srcImage, const Image& dstImage) const;
    void BlitImage(const Image& srcImage, const Swapchain& swapchain) const;

    // support dynamic sets
    void BindDescriptorSets(
        VkDescriptorSet descriptorSet,
        const Pipeline& pipeline) const;

    void BindVertexBuffers(VkBuffer buffer) const;
    void BindIndexBuffer(VkBuffer buffer) const;

    template <typename T>
    void PushConstants(
        VkPipelineLayout layout,
        VkShaderStageFlags shaderStage,
        const std::uint32_t offset,
        const T& data) const
    {
        // TODO: consifer reading layout and shaders from bind pipeline

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

    void SetViewport(const VkViewport& viewport) const;
    void SetScissor(const VkRect2D& scissor) const;
    void SetCullMode(VkCullModeFlags cullMode) const;

    void BeginDebugLabel(const char* pLabelName, const Color& color) const;
    void InsertDebugLabel(const char* pLabelName, const Color& color) const;
    void EndDebugLabel() const;

    VkCommandBuffer GetHandle() const;

private:
    VkCommandBuffer m_handle{ VK_NULL_HANDLE };
    const CommandPool* m_commandPool{ nullptr };
};
}
