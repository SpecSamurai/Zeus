#include "LinearAllocator.hpp"

#include "Allocator.hpp"
#include "memory.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace Zeus
{
LinearAllocator::LinearAllocator(
    const std::size_t sizeBytes,
    void* const start) noexcept
    : Allocator{ sizeBytes, start },
      m_current{ const_cast<void*>(start) }
// , m_current{start}
{
}

LinearAllocator::LinearAllocator(LinearAllocator&& other) noexcept
    : Allocator{ std::move(other) },
      m_current{ other.m_current }
{
    other.m_current = nullptr;
}

LinearAllocator::~LinearAllocator() noexcept
{
    Clear();
}

LinearAllocator& LinearAllocator::operator=(LinearAllocator&& rhs) noexcept
{
    Allocator::operator=(std::move(rhs));

    m_current = rhs.m_current;
    rhs.m_current = nullptr;

    return *this;
}

void* LinearAllocator::Allocate(
    const std::size_t& size,
    const std::uintptr_t& alignment)
{
    assert(size > 0 && alignment > 0);

    std::size_t adjustment{ alignForwardAdjustment(m_current, alignment) };

    assert(m_usedBytes + adjustment + size <= m_size);

    void* alignedAddr{ addPtr(m_current, adjustment) };

    m_current = addPtr(alignedAddr, size);

    m_usedBytes = reinterpret_cast<std::uintptr_t>(m_current) -
                  reinterpret_cast<std::uintptr_t>(m_start);

    ++m_numAllocations;

    return alignedAddr;
}

void LinearAllocator::Free([[maybe_unused]] void* const ptr) noexcept
{
}

void LinearAllocator::Rewind(void* const mark) noexcept
{
    assert(m_current >= mark && m_start <= mark);

    m_current = mark;
    m_usedBytes = reinterpret_cast<std::uintptr_t>(m_current) -
                  reinterpret_cast<std::uintptr_t>(m_start);
}

void LinearAllocator::Clear() noexcept
{
    m_numAllocations = 0;
    m_usedBytes = 0;
    m_current = m_start;
    // m_current = const_cast<void*>(m_start);
}

const void* LinearAllocator::GetCurrent() const noexcept
{
    return m_current;
}
}
