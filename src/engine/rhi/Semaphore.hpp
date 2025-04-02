#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <limits>
#include <string_view>

namespace Zeus
{
class Semaphore
{
public:
    Semaphore() = default;
    Semaphore(std::string_view name, bool isTimeline);

    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;

    Semaphore(Semaphore&& other) noexcept;
    Semaphore& operator=(Semaphore&& other);

    ~Semaphore();
    void Destroy();

    // timeout_ns - timeout in nanoseconds
    void Wait(
        const std::uint64_t value,
        const std::uint64_t timeout_ns =
            std::numeric_limits<std::uint64_t>::max()) const;

    void Signal(const std::uint64_t value) const;

    std::uint64_t GetValue() const;
    VkSemaphore GetHandle() const;

    inline constexpr std::string_view GetName() const
    {
        return m_name;
    }

private:
    VkSemaphore m_handle{ VK_NULL_HANDLE };
    std::string_view m_name;
    bool m_isTimeline{ false };
};
}
