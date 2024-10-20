#pragma once

#include "Definitions.hpp"
#include "Fence.hpp"
#include "Handle.hpp"
#include "Semaphore.hpp"
#include "SwapchainBuilder.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>

namespace Zeus
{
class Queue : public Handle
{
public:
    Queue(const QueueType type, const char* name = nullptr);

    ~Queue();

    void Wait();

    void Submit(
        VkCommandBuffer commandBuffer,
        const Semaphore& waitSemaphore,
        const Semaphore& signalSemaphore,
        const Fence& fence);

    void Present(
        const Swapchain& swapchain,
        std::uint32_t swapchainImageIndex,
        const Semaphore& waitSemaphores);

    const VkQueue& GetHandle() const;
    QueueType GetType() const;
    std::uint32_t GetFamilyIndex() const;

private:
    VkQueue m_handle{ VK_NULL_HANDLE };
    QueueType m_type;
    std::uint32_t m_family;
};
}
