#pragma once

#include <cstddef>
#include <cstdint>

namespace Zeus
{
struct AllocationHeader
{
    std::size_t size;
    std::uintptr_t adjustment;
};
}
