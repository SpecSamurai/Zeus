#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <limits>

namespace Zeus
{
class Fence
{
public:
    Fence(bool signaled = false, const char* name = nullptr);
    ~Fence();

    // timeout_ns - timeout in nanoseconds
    bool Wait(
        std::uint64_t timeout_ns = std::numeric_limits<std::uint64_t>::max());
    void Reset();

    const VkFence& GetHandle() const;
    bool Signaled() const;

private:
    VkFence m_handle{ VK_NULL_HANDLE };
};
}
