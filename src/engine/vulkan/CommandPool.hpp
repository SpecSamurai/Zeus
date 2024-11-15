#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <vector>

namespace Zeus
{
class CommandPool
{
public:
    CommandPool() = default;
    CommandPool(
        std::uint32_t queueFamilyIndex,
        VkCommandPoolCreateFlagBits createFlags = {},
        const char* name = nullptr);

    ~CommandPool();

    VkCommandBuffer AllocateBuffer(
        VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY) const;

    void AllocateBuffers(
        std::vector<VkCommandBuffer>& commandBuffers,
        VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY) const;

    void Reset(VkCommandPoolResetFlags flags = 0);

    VkCommandPool GetHandle() const;

private:
    VkCommandPool m_handle{ VK_NULL_HANDLE };
};
}
