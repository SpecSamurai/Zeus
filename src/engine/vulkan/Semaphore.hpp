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
    Semaphore(bool isTimeline, std::string_view name = "");

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
            std::numeric_limits<std::uint64_t>::max());
    void Signal(const std::uint64_t value);

    std::uint64_t GetValue() const;
    const VkSemaphore& GetHandle() const;

private:
    VkSemaphore m_handle{ VK_NULL_HANDLE };
    bool m_isTimeline{ false };
};
}
