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
    Fence(std::string_view name, bool signaled);

    Fence(const Fence&) = delete;
    Fence& operator=(const Fence&) = delete;

    Fence(Fence&& other) noexcept;
    Fence& operator=(Fence&& other);

    ~Fence();
    void Destroy();

    // timeout_ns - timeout in nanoseconds
    bool Wait(
        std::uint64_t timeout_ns =
            std::numeric_limits<std::uint64_t>::max()) const;
    void Reset() const;

    bool Signaled() const;

    VkFence GetHandle() const;

    inline constexpr std::string_view GetName() const
    {
        return m_name;
    }

private:
    VkFence m_handle{ VK_NULL_HANDLE };

    std::string_view m_name;
};
}
