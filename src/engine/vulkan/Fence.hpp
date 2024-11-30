#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <limits>
#include <string_view>

namespace Zeus
{
class Fence
{
public:
    Fence() = default;
    Fence(bool signaled, std::string_view name = "");

    Fence(const Fence&) = delete;
    Fence& operator=(const Fence&) = delete;

    Fence(Fence&& other) noexcept;
    Fence& operator=(Fence&& other);

    ~Fence();
    void Destroy();

    // timeout_ns - timeout in nanoseconds
    bool Wait(
        std::uint64_t timeout_ns = std::numeric_limits<std::uint64_t>::max());
    void Reset();

    bool Signaled() const;

    const VkFence& GetHandle() const;

private:
    VkFence m_handle{ VK_NULL_HANDLE };
};
}
