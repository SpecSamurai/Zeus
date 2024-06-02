#pragma once

#include <cstddef>

namespace Zeus
{
struct FreeBlock
{
    std::size_t size;
    FreeBlock* next;
};
}
