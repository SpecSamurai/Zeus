#include "CommandPool.hpp"

#include "VkContext.hpp"
#include "vulkan/vulkan_debug.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <string_view>
#include <vector>

namespace Zeus
{
CommandPool::CommandPool(
    std::uint32_t queueFamilyIndex,
    VkCommandPoolCreateFlagBits createFlags,
    std::string_view name)
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

    VkContext::SetDebugName(VK_OBJECT_TYPE_COMMAND_POOL, m_handle, name);
}

CommandPool::CommandPool(CommandPool&& other) noexcept
    : m_handle{ other.m_handle }
{
    other.m_handle = VK_NULL_HANDLE;
}

CommandPool& CommandPool::operator=(CommandPool&& other)
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

CommandPool::~CommandPool()
{
    if (m_handle == VK_NULL_HANDLE)
        return;

    VkContext::GetDeletionQueue().Add(ResourceType::CommandPool, m_handle);
    m_handle = VK_NULL_HANDLE;
}

void CommandPool::Destroy()
{
    vkDestroyCommandPool(
        VkContext::GetLogicalDevice(),
        m_handle,
        allocationCallbacks.get());
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

void CommandPool::Reset(VkCommandPoolResetFlags flags) const
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
