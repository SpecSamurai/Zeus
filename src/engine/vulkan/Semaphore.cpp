#include "Semaphore.hpp"

#include "VkContext.hpp"
#include "rhi/vulkan_debug.hpp"
#include "rhi/vulkan_sync.hpp"

#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cstdint>

namespace Zeus
{
Semaphore::Semaphore(bool isTimeline, const char* name)
    : m_isTimeline{ isTimeline }
{
    createVkSemaphore(VkContext::GetLogicalDevice(), &m_handle, m_isTimeline);

    VkContext::SetDebugName(VK_OBJECT_TYPE_SEMAPHORE, m_handle, name);
}

Semaphore::~Semaphore()
{
    if (m_handle == VK_NULL_HANDLE)
        return;

    VkContext::GetDeletionQueue().Add(ResourceType::Semaphore, m_handle);
    m_handle = VK_NULL_HANDLE;
}

void Semaphore::Destroy()
{
    vkDestroySemaphore(
        VkContext::GetLogicalDevice(),
        m_handle,
        allocationCallbacks.get());
    m_handle = VK_NULL_HANDLE;
}

void Semaphore::Wait(const std::uint64_t value, const std::uint64_t timeout_ns)
{
    assert(m_isTimeline);

    waitVkSemaphores(
        VkContext::GetLogicalDevice(),
        &m_handle,
        value,
        timeout_ns);
}

void Semaphore::Signal(const std::uint64_t value)
{
    assert(m_isTimeline);

    signalVkSemaphores(VkContext::GetLogicalDevice(), m_handle, value);
}

std::uint64_t Semaphore::GetValue() const
{
    assert(m_isTimeline);

    std::uint64_t value{ 0 };

    VKCHECK(
        vkGetSemaphoreCounterValue(
            VkContext::GetLogicalDevice(),
            m_handle,
            &value),
        "Failed to get Semaphore counter value");

    return value;
}

const VkSemaphore& Semaphore::GetHandle() const
{
    return m_handle;
}
}
