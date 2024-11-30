#include "Fence.hpp"

#include "VkContext.hpp"
#include "rhi/vulkan_debug.hpp"
#include "rhi/vulkan_sync.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <string_view>

namespace Zeus
{
Fence::Fence(bool signaled, std::string_view name)
{
    createVkFence(VkContext::GetLogicalDevice(), signaled, &m_handle);

    VkContext::SetDebugName(VK_OBJECT_TYPE_FENCE, m_handle, name);
}

Fence::Fence(Fence&& other) noexcept : m_handle{ other.m_handle }
{
    other.m_handle = VK_NULL_HANDLE;
}

Fence& Fence::operator=(Fence&& other)
{
    if (this != &other)
    {
        if (m_handle != VK_NULL_HANDLE)
        {
            Destroy();
        }

        m_handle = other.m_handle;
        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}

Fence::~Fence()
{
    if (m_handle == VK_NULL_HANDLE)
        return;

    VkContext::GetDeletionQueue().Add(ResourceType::Fence, m_handle);
    m_handle = VK_NULL_HANDLE;
}

void Fence::Destroy()
{
    vkDestroyFence(
        VkContext::GetLogicalDevice(),
        m_handle,
        allocationCallbacks.get());
    m_handle = VK_NULL_HANDLE;
}

bool Fence::Wait(std::uint64_t timeout_ns)
{
    return vkWaitForFences(
               VkContext::GetLogicalDevice(),
               1,
               &m_handle,
               VK_TRUE,
               timeout_ns) == VK_SUCCESS;
}

void Fence::Reset()
{
    VKCHECK(
        vkResetFences(VkContext::GetLogicalDevice(), 1, &m_handle),
        "Failed to reset fence");
}

bool Fence::Signaled() const
{
    return vkGetFenceStatus(VkContext::GetLogicalDevice(), m_handle) ==
           VK_SUCCESS;
}

const VkFence& Fence::GetHandle() const
{
    return m_handle;
}
}
