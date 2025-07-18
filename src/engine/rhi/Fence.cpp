#include "Fence.hpp"

#include "VkContext.hpp"
#include "vulkan/vulkan_debug.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <string_view>

namespace Zeus
{
Fence::Fence(std::string_view name, bool signaled) : m_name{ name }
{
    VkFenceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    if (signaled)
    {
        createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    }

    VKCHECK(
        vkCreateFence(
            VkContext::LogicalDevice(),
            &createInfo,
            allocationCallbacks.get(),
            &m_handle),
        "Failed to create Fence.");

    VkContext::SetDebugName(VK_OBJECT_TYPE_FENCE, m_handle, m_name);
}

Fence::Fence(Fence&& other) noexcept
    : m_handle{ other.m_handle },
      m_name{ other.m_name }
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
        m_name = other.m_name;

        other.m_handle = VK_NULL_HANDLE;
    }

    return *this;
}

Fence::~Fence()
{
    if (m_handle == VK_NULL_HANDLE)
        return;

    VkContext::DeletionQueue().Add(ResourceType::Fence, m_handle);
    m_handle = VK_NULL_HANDLE;
}

void Fence::Destroy()
{
    vkDestroyFence(
        VkContext::LogicalDevice(),
        m_handle,
        allocationCallbacks.get());
    m_handle = VK_NULL_HANDLE;
}

bool Fence::Wait(std::uint64_t timeout_ns) const
{
    return vkWaitForFences(
               VkContext::LogicalDevice(),
               1,
               &m_handle,
               VK_TRUE,
               timeout_ns) == VK_SUCCESS;
}

void Fence::Reset() const
{
    VKCHECK(
        vkResetFences(VkContext::LogicalDevice(), 1, &m_handle),
        "Failed to reset fence");
}

bool Fence::Signaled() const
{
    return vkGetFenceStatus(VkContext::LogicalDevice(), m_handle) ==
           VK_SUCCESS;
}

VkFence Fence::GetHandle() const
{
    return m_handle;
}
}
