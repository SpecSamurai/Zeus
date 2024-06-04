#pragma once

#include "Allocator.hpp"

#include <cstdint>

namespace Zeus
{
class LinearAllocator : public Allocator
{
public:
    LinearAllocator(const std::size_t sizeBytes, void* const start) noexcept;

    LinearAllocator(const LinearAllocator&) = delete;
    LinearAllocator& operator=(const LinearAllocator&) = delete;
    LinearAllocator(LinearAllocator&&) noexcept;
    LinearAllocator& operator=(LinearAllocator&&) noexcept;

    virtual ~LinearAllocator() noexcept;

    virtual void* Allocate(
        const std::size_t& size,
        const std::uintptr_t& alignment = sizeof(std::intptr_t)) override;

    virtual void Free(void* const ptr) noexcept override final;

    virtual void Rewind(void* const mark) noexcept;
    virtual void Clear() noexcept;

    const void* GetCurrent() const noexcept;

protected:
    void* m_current; // pointer to the free section of memory
};
}
