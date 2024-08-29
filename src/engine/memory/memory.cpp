#include "memory.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>

namespace Zeus
{
// formula increase an offset (address) to a next address aligned to the
// alignment formula makes sense only when alignment is a power of 2
std::size_t alignForwardAdjustment(
    const void* const ptr,
    const std::size_t& alignment) noexcept
{
    const auto offset{ reinterpret_cast<std::uintptr_t>(ptr) };
    const auto aligned{ (offset - 1u + alignment) & -alignment };

    return aligned - offset;
}

void* addPtr(const void* const ptr, const std::uintptr_t& offset) noexcept
{
    return reinterpret_cast<void*>(
        reinterpret_cast<std::uintptr_t>(ptr) + offset);
}

void* subPtr(const void* const ptr, const std::uintptr_t& offset) noexcept
{
    return reinterpret_cast<void*>(
        reinterpret_cast<std::uintptr_t>(ptr) - offset);
}
}
