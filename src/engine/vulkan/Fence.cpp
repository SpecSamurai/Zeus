#include "Fence.hpp"

#include "Handle.hpp"
#include "VulkanContext.hpp"
#include "api/vulkan_debug.hpp"
#include "api/vulkan_sync.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>

namespace Zeus
{
Fence::Fence(bool signaled, const char* name) : Handle(name)
{
    createVkFence(
        VulkanContext::Get().GetDevice().logicalDevice,
        signaled,
        m_handle);

#ifndef NDEBUG
    if (m_name != nullptr)
    {
        setDebugUtilsObjectNameEXT(
            VulkanContext::Get().GetDevice().logicalDevice,
            VK_OBJECT_TYPE_FENCE,
            reinterpret_cast<std::uint64_t>(m_handle),
            m_name);
    }
#endif
}

Fence::~Fence()
{
    if (m_handle == VK_NULL_HANDLE)
        return;

    VulkanContext::Get().GetDevice().deletionQueue.Add(
        ResourceType::Fence,
        m_handle);

    m_handle = VK_NULL_HANDLE;
}

bool Fence::Wait(std::uint64_t timeout_ns)
{
    return vkWaitForFences(
               VulkanContext::Get().GetDevice().logicalDevice,
               1,
               &m_handle,
               VK_TRUE,
               timeout_ns) == VK_SUCCESS;
}

void Fence::Reset()
{
    VKCHECK(
        vkResetFences(
            VulkanContext::Get().GetDevice().logicalDevice,
            1,
            &m_handle),
        "Failed to reset fence");
}

VkFence Fence::GetHandle() const
{
    return m_handle;
}

bool Fence::Signaled() const
{
    return vkGetFenceStatus(
               VulkanContext::Get().GetDevice().logicalDevice,
               m_handle) == VK_SUCCESS;
}
}
