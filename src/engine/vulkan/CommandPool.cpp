#include "CommandPool.hpp"

#include "VkContext.hpp"
#include "rhi/vulkan_debug.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
CommandPool::CommandPool(
    std::uint32_t queueFamilyIndex,
    VkCommandPoolCreateFlagBits createFlags,
    const char* name)
{
    VkCommandPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = createFlags;
    createInfo.queueFamilyIndex = queueFamilyIndex;

    VKCHECK(
        vkCreateCommandPool(
            VkContext::GetLogicalDevice(),
            &createInfo,
            allocationCallbacks.get(),
            &m_handle),
        "Failed to create command pool.");

#ifndef NDEBUG
    if (name != nullptr)
    {
        setDebugUtilsObjectNameEXT(
            VkContext::GetLogicalDevice(),
            VK_OBJECT_TYPE_COMMAND_POOL,
            reinterpret_cast<std::uint64_t>(m_handle),
            name);
    }
#endif
}

CommandPool::~CommandPool()
{
    if (m_handle == VK_NULL_HANDLE)
        return;

    VkContext::GetDeletionQueue().Add(ResourceType::CommandPool, m_handle);
    m_handle = VK_NULL_HANDLE;
}

VkCommandBuffer CommandPool::AllocateBuffer(VkCommandBufferLevel level) const
{
    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandPool = m_handle;
    allocateInfo.level = level;
    allocateInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    VKCHECK(
        vkAllocateCommandBuffers(
            VkContext::GetLogicalDevice(),
            &allocateInfo,
            &commandBuffer),
        "Failed to allocate command buffers.");

    return commandBuffer;
}

void CommandPool::AllocateBuffers(
    std::vector<VkCommandBuffer>& commandBuffers,
    VkCommandBufferLevel level) const
{
    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandPool = m_handle;
    allocateInfo.level = level;
    allocateInfo.commandBufferCount =
        static_cast<std::uint32_t>(commandBuffers.size());

    VKCHECK(
        vkAllocateCommandBuffers(
            VkContext::GetLogicalDevice(),
            &allocateInfo,
            commandBuffers.data()),
        "Failed to allocate command buffers.");
}

void CommandPool::Reset(VkCommandPoolResetFlags flags)
{
    VKCHECK(
        vkResetCommandPool(VkContext::GetLogicalDevice(), m_handle, flags),
        "Failed to reset command pool.");
}

VkCommandPool CommandPool::GetHandle() const
{
    return m_handle;
}
}
