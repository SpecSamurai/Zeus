#pragma once

#include <cstddef>
#include <cstdint>

namespace Zeus
{
class Allocator
{
public:
    Allocator(const std::size_t sizeBytes, void* const start) noexcept;

    // You can’t have two copies making changes
    // to the underlying m_start and m_usedBytes
    // and giving away allocations (because then the same memory will be given
    // out twice).
    Allocator(const Allocator&) = delete;
    Allocator& operator=(Allocator&) = delete;
    Allocator(Allocator&&) noexcept;
    Allocator& operator=(Allocator&&) noexcept;

    virtual ~Allocator() noexcept;

    // If the allocation requested has a specific alignment need
    // we just move far enough ahead to meet that requirement.
    virtual void* Allocate(
        const std::size_t& size,
        const std::uintptr_t& alignment = sizeof(std::intptr_t)) = 0;

    virtual void Free(void* const ptr) = 0;

    const std::size_t& GetSize() const noexcept;
    const std::size_t& GetUsedBytes() const noexcept;
    const std::size_t& GetNumAllocations() const noexcept;

    const void* GetStart() const noexcept;

protected:
    std::size_t m_size;           // is how much data is at m_start
    std::size_t m_usedBytes;      // how much of m_size is still available
    std::size_t m_numAllocations; // how many allocations(monitoring)

    void* m_start; // pointer to the start of the memory available
};
}
