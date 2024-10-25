#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <limits>

namespace Zeus
{
class Semaphore
{
public:
    Semaphore(bool isTimeline = false, const char* name = nullptr);
    ~Semaphore();

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
    bool m_isTimeline;
};
}
