#pragma once

#include "Definitions.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <string_view>

namespace Zeus
{
class Queue
{
public:
    Queue() = default;
    Queue(std::string_view name, const QueueType type, std::uint32_t family);

    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;

    Queue(Queue&& other) noexcept;
    Queue& operator=(Queue&& other);

    void Wait() const;

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

    inline constexpr std::string_view GetName() const
    {
        return m_name;
    }

private:
    VkQueue m_handle{ VK_NULL_HANDLE };
    std::string_view m_name;

    std::uint32_t m_family;
    QueueType m_type;
};
}
