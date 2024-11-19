#pragma once

#include "Definitions.hpp"
#include "Fence.hpp"
#include "Semaphore.hpp"
#include "Swapchain.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>

namespace Zeus
{
class Queue
{
public:
    Queue() = default;
    Queue(
        const QueueType type,
        std::uint32_t family,
        const char* name = nullptr);
    ~Queue();

    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;

    Queue(Queue&& other) noexcept;
    Queue& operator=(Queue&& other);

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
