#include "Semaphore.hpp"

#include "Handle.hpp"
#include "VulkanContext.hpp"
#include "api/vulkan_debug.hpp"
#include "api/vulkan_sync.hpp"
#include "vulkan/Definitions.hpp"

#include <vulkan/vulkan_core.h>

#include <cassert>
#include <cstdint>

namespace Zeus
{
Semaphore::Semaphore(bool isTimeline, const char* name)
    : Handle(name),
      m_isTimeline{ isTimeline }
{
    createVkSemaphore(
        VulkanContext::Get().GetDevice().logicalDevice,
        m_handle,
        m_isTimeline);

#ifndef NDEBUG
    if (m_name != nullptr)
    {
        setDebugUtilsObjectNameEXT(
            VulkanContext::Get().GetDevice().logicalDevice,
            VK_OBJECT_TYPE_SEMAPHORE,
            reinterpret_cast<std::uint64_t>(m_handle),
            m_name);
    }
#endif
}

Semaphore::~Semaphore()
{
    if (m_handle == VK_NULL_HANDLE)
        return;

    VulkanContext::Get().GetDevice().deletionQueue.Add(
        ResourceType::Semaphore,
        m_handle);

    m_handle = VK_NULL_HANDLE;
}

void Semaphore::Wait(const std::uint64_t value, const std::uint64_t timeout_ns)
{
    assert(m_isTimeline);

    waitVkSemaphores(
        VulkanContext::Get().GetDevice().logicalDevice,
        m_handle,
        value,
        timeout_ns);
}

void Semaphore::Signal(const std::uint64_t value)
{
    assert(m_isTimeline);

    signalVkSemaphores(
        VulkanContext::Get().GetDevice().logicalDevice,
        m_handle,
        value);
}

std::uint64_t Semaphore::GetValue() const
{
    assert(m_isTimeline);

    std::uint64_t value{ 0 };

    VKCHECK(
        vkGetSemaphoreCounterValue(
            VulkanContext::Get().GetDevice().logicalDevice,
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
