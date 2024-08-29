#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>

namespace Zeus
{
template <typename T>
bool isPowerOf2(T x)
{
    assert(x >= 0);
    return (x & (x - 1)) == 0;
}

// https://en.wikipedia.org/wiki/Data_structure_alignment
// Alternative: (offset + (align - 1)) & ~(align - 1)
// Produce the correct values providing the alignment is a power of two.
// Aligns given value up to nearest multiply of align value.
// For example: alignUp(11, 8) = 16
template <typename T>
T alignUp(T value, T alignment)
{
    assert(isPowerOf2(alignment));
    return (value + (alignment - 1)) & -alignment;
}

// Aligns given value down to nearest multiply of align value.
// For example: alignDown(11, 8) = 8
template <typename T>
T alignDown(T value, T alignment)
{
    assert(isPowerOf2(alignment));
    return value & ~(alignment - 1);
}

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
