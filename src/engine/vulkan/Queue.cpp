#include "Queue.hpp"

#include "Definitions.hpp"
#include "VkContext.hpp"
#include "rhi/vulkan_debug.hpp"

#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cstdint>
#include <string_view>

namespace Zeus
{
Queue::Queue(const QueueType type, std::uint32_t family, std::string_view name)
    : m_type{ type },
      m_family{ family }
{
    vkGetDeviceQueue(VkContext::GetLogicalDevice(), family, 0, &m_handle);

    VkContext::SetDebugName(VK_OBJECT_TYPE_QUEUE, m_handle, name);
}

Queue::Queue(Queue&& other) noexcept
    : m_handle{ other.m_handle },
      m_type{ other.m_type },
      m_family{ other.m_family }
{
    other.m_handle = VK_NULL_HANDLE;
}

Queue& Queue::operator=(Queue&& other)
{
    if (this != &other)
    {
        m_handle = other.m_handle;
        m_type = other.m_type;
        m_family = other.m_family;

        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}

void Queue::Wait() const
{
    VKCHECK(vkQueueWaitIdle(m_handle), "Failed to wait for Queue");
}

void Queue::Submit(
    VkFence fence,
    std::uint32_t waitSemaphoreInfoCount,
    const VkSemaphoreSubmitInfo* pWaitSemaphoreInfos,
    std::uint32_t commandBufferInfoCount,
    const VkCommandBufferSubmitInfo* pCommandBufferInfos,
    std::uint32_t signalSemaphoreInfoCount,
    const VkSemaphoreSubmitInfo* pSignalSemaphoreInfos) const
{
    assert(m_type == QueueType::Graphics || m_type == QueueType::Transfer);

    VkSubmitInfo2 submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;

    submitInfo.waitSemaphoreInfoCount = waitSemaphoreInfoCount;
    submitInfo.pWaitSemaphoreInfos = pWaitSemaphoreInfos;

    submitInfo.commandBufferInfoCount = commandBufferInfoCount;
    submitInfo.pCommandBufferInfos = pCommandBufferInfos;

    submitInfo.signalSemaphoreInfoCount = signalSemaphoreInfoCount;
    submitInfo.pSignalSemaphoreInfos = pSignalSemaphoreInfos;

    VKCHECK(
        vkQueueSubmit2(m_handle, 1, &submitInfo, fence),
        "Failed to submit command buffer.");
}

QueueType Queue::GetType() const
{
    return m_type;
}

VkQueue Queue::GetHandle() const
{
    return m_handle;
}

std::uint32_t Queue::GetFamilyIndex() const
{
    return m_family;
}
}
