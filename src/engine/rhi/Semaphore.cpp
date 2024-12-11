#include "Semaphore.hpp"

#include "VkContext.hpp"
#include "vulkan/vulkan_debug.hpp"

#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cstdint>
#include <string_view>

namespace Zeus
{
Semaphore::Semaphore(bool isTimeline, std::string_view name)
    : m_isTimeline{ isTimeline }
{
    VkSemaphoreTypeCreateInfo typeCreateInfo{};
    typeCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    typeCreateInfo.pNext = nullptr;
    typeCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    typeCreateInfo.initialValue = 0;

    VkSemaphoreCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    createInfo.pNext = m_isTimeline ? &typeCreateInfo : nullptr;
    createInfo.flags = 0;

    VKCHECK(
        vkCreateSemaphore(
            VkContext::GetLogicalDevice(),
            &createInfo,
            allocationCallbacks.get(),
            &m_handle),
        "Failed to create Semaphore.");

    VkContext::SetDebugName(VK_OBJECT_TYPE_SEMAPHORE, m_handle, name);
}

Semaphore::Semaphore(Semaphore&& other) noexcept
    : m_handle{ other.m_handle },
      m_isTimeline{ other.m_isTimeline }
{
    other.m_handle = VK_NULL_HANDLE;
}

Semaphore& Semaphore::operator=(Semaphore&& other)
{
    if (this != &other)
    {
        if (m_handle != VK_NULL_HANDLE)
        {
            Destroy();
        }

        m_handle = other.m_handle;
        m_isTimeline = other.m_isTimeline;

        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
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
    const
{
    assert(m_isTimeline);

    VkSemaphoreWaitInfo waitInfo{};
    waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
    waitInfo.pNext = nullptr;
    waitInfo.flags = 0;
    waitInfo.semaphoreCount = 1;
    waitInfo.pSemaphores = &m_handle;
    waitInfo.pValues = &value;

    VKCHECK(
        vkWaitSemaphores(VkContext::GetLogicalDevice(), &waitInfo, timeout_ns),
        "Failed to wait for Semaphore.");
}

void Semaphore::Signal(const std::uint64_t value) const
{
    assert(m_isTimeline);

    VkSemaphoreSignalInfo signalInfo{};
    signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
    signalInfo.pNext = nullptr;
    signalInfo.semaphore = m_handle;
    signalInfo.value = value;

    VKCHECK(
        vkSignalSemaphore(VkContext::GetLogicalDevice(), &signalInfo),
        "Failed to signal Semaphore.");
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

VkSemaphore Semaphore::GetHandle() const
{
    return m_handle;
}
}
