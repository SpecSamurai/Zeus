#pragma once

#include <cstddef>
#include <cstdint>

namespace Zeus
{
// https://en.wikipedia.org/wiki/Data_structure_alignment
// Alternative: (offset + (align - 1)) & ~(align - 1)
// Produce the correct values providing the alignment is a power of two
std::size_t align(std::size_t size, std::size_t alignment);

void* addPtr(const void* const ptr, const std::uintptr_t& offset) noexcept;

void* subPtr(const void* const ptr, const std::uintptr_t& offset) noexcept;

std::size_t alignForwardAdjustment(
    const void* const ptr,
    const std::size_t& alignment) noexcept;

template <typename T>
std::size_t alignForwardAdjustmentWithHeader(
    const void* const ptr,
    const std::size_t& alignment) noexcept
{
    auto fixedAlignment{ alignof(T) > alignment ? alignof(T) : alignment };

    std::size_t adjustment =
        sizeof(T) +
        alignForwardAdjustment(addPtr(ptr, sizeof(T)), fixedAlignment);

    return adjustment;
}
}
