#pragma once

#include "Allocator.hpp"
#include "FreeBlock.hpp"

#include <cstddef>
#include <cstdint>

namespace Zeus
{
class FreeListAllocator : public Allocator
{
public:
    FreeListAllocator(const std::size_t sizeBytes, void* const start) noexcept;

    FreeListAllocator(const FreeListAllocator&) = delete;
    FreeListAllocator& operator=(const FreeListAllocator&) = delete;
    FreeListAllocator(FreeListAllocator&&) noexcept;
    FreeListAllocator& operator=(FreeListAllocator&&) noexcept;

    virtual ~FreeListAllocator() noexcept;

    virtual void* Allocate(
        const std::size_t& size,
        const std::uintptr_t& alignment = sizeof(std::uintptr_t)) override;

    virtual void Free(void* const ptr) noexcept override final;

protected:
    FreeBlock* m_freeBlocks;
};
}
