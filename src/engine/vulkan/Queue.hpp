#pragma once

#include "Definitions.hpp"

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

    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;

    Queue(Queue&& other) noexcept;
    Queue& operator=(Queue&& other);

    void Wait();

    void Submit(
        VkFence fence,
        std::uint32_t waitSemaphoreInfoCount = 0,
        const VkSemaphoreSubmitInfo* pWaitSemaphoreInfos = nullptr,
        std::uint32_t commandBufferInfoCount = 0,
        const VkCommandBufferSubmitInfo* pCommandBufferInfos = nullptr,
        std::uint32_t signalSemaphoreInfoCount = 0,
        const VkSemaphoreSubmitInfo* pSignalSemaphoreInfos = nullptr) const;

    QueueType GetType() const;
    VkQueue GetHandle() const;
    std::uint32_t GetFamilyIndex() const;

private:
    VkQueue m_handle{ VK_NULL_HANDLE };
    QueueType m_type;
    std::uint32_t m_family;
};
}
