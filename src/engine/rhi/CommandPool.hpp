#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <string_view>
#include <vector>

namespace Zeus
{
class CommandPool
{
public:
    CommandPool() = default;
    CommandPool(
        std::string_view name,
        std::uint32_t queueFamilyIndex,
        VkCommandPoolCreateFlagBits createFlags = {});

    CommandPool(const CommandPool&) = delete;
    CommandPool& operator=(const CommandPool&) = delete;

    CommandPool(CommandPool&& other) noexcept;
    CommandPool& operator=(CommandPool&& other);

    ~CommandPool();

    void Destroy();

    VkCommandBuffer AllocateBuffer(
        VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY) const;

    void AllocateBuffers(
        std::vector<VkCommandBuffer>& commandBuffers,
        VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY) const;

    void Reset(VkCommandPoolResetFlags flags = 0) const;

    VkCommandPool GetHandle() const;

    inline constexpr std::string_view GetName() const
    {
        return m_name;
    }

private:
    VkCommandPool m_handle{ VK_NULL_HANDLE };
    std::string_view m_name;
};
}
