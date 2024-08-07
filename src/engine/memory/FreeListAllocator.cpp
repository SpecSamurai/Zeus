#include "FreeListAllocator.hpp"

#include "AllocationHeader.hpp"
#include "Allocator.hpp"
#include "FreeBlock.hpp"
#include "memory.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace Zeus
{
FreeListAllocator::FreeListAllocator(
    const std::size_t sizeBytes,
    void* const start) noexcept
    : Allocator{ sizeBytes, start },
      m_freeBlocks{ reinterpret_cast<FreeBlock*>(start) }
{
    m_freeBlocks->size = sizeBytes;
    m_freeBlocks->next = nullptr;
}

FreeListAllocator::FreeListAllocator(FreeListAllocator&& other) noexcept
    : Allocator(std::move(other)),
      m_freeBlocks{ other.m_freeBlocks }
{
    other.m_freeBlocks = nullptr;
}

FreeListAllocator& FreeListAllocator::operator=(
    FreeListAllocator&& rhs) noexcept
{
    Allocator::operator=(std::move(rhs));

    m_freeBlocks = rhs.m_freeBlocks;
    rhs.m_freeBlocks = nullptr;

    return *this;
}

FreeListAllocator::~FreeListAllocator() noexcept
{
    m_freeBlocks = nullptr;
    m_numAllocations = 0;
    m_usedBytes = 0;
}

void* FreeListAllocator::Allocate(
    const std::size_t& size,
    const std::uintptr_t& alignment)
{
    // find the best fitting block that is at least size + header and then
    // aligned. "best fitting" is the smallest possible block unless we find
    // one that is exactly the right size
    assert(size > 0 && alignment > 0);

    FreeBlock* prevFreeBlock{ nullptr };
    FreeBlock* freeBlock{ m_freeBlocks };

    FreeBlock* bestFitPrev{ nullptr };
    FreeBlock* bestFit{ nullptr };

    std::uintptr_t bestFitAdjustment{ 0u };
    std::size_t bestFitTotalSize{ 0u };

    while (freeBlock != nullptr)
    {
        std::uintptr_t adjustment =
            alignForwardAdjustmentWithHeader<AllocationHeader>(
                freeBlock,
                alignment);

        // totalSize must store data + header, and be aligned
        std::size_t totalSize = size + adjustment;

        // is this block a better fit than the previously big enough block
        // "better fit" means it's smaller, so we favour splitting small blocks
        if (freeBlock->size > totalSize &&
            (bestFit == nullptr || freeBlock->size < bestFit->size))
        {
            bestFitPrev = prevFreeBlock;
            bestFit = freeBlock;
            bestFitAdjustment = adjustment;
            bestFitTotalSize = totalSize;

            if (freeBlock->size == totalSize)
            {
                break;
            }
        }

        prevFreeBlock = freeBlock;
        freeBlock = freeBlock->next;
    }

    assert(bestFit != nullptr);

    if (bestFit->size - bestFitTotalSize <= sizeof(AllocationHeader))
    {
        // this bestFit cannot be split into two because the remainder wouldn't
        // be enough to hold another allocation (need at least the size of the
        // header + 1 byte)

        // adjust the size of this alocation to be the entireity of the
        // freeblock
        bestFitTotalSize = bestFit->size;

        // remove the free block from the list
        if (bestFitPrev != nullptr)
            bestFitPrev->next = bestFit->next;
        else
            m_freeBlocks = bestFit->next;
    }
    else
    {
        assert(bestFitTotalSize > sizeof(FreeBlock)); // no needed?

        // split the best fit block into how much we need for this allocation
        // and a new block of the remainder

        // new block starts at bestFit + the size we need
        FreeBlock* newBlock =
            reinterpret_cast<FreeBlock*>(addPtr(bestFit, bestFitTotalSize));

        // it has the remaining size
        newBlock->size = bestFit->size - bestFitTotalSize;
        // it's next is the block we are splitting's next
        newBlock->next = bestFit->next;

        // insert the new block into the free list
        if (bestFitPrev != nullptr)
            bestFitPrev->next = newBlock;
        else
            m_freeBlocks = newBlock;
    }

    std::uintptr_t alignedAddr =
        reinterpret_cast<std::uintptr_t>(bestFit) + bestFitAdjustment;

    // shift it back into the space we allowed for the header and pupulate the
    // header
    AllocationHeader* header = reinterpret_cast<AllocationHeader*>(
        alignedAddr - sizeof(AllocationHeader));

    header->size = bestFitTotalSize;
    header->adjustment = bestFitAdjustment;

    m_usedBytes += bestFitTotalSize;
    ++m_numAllocations;

    return reinterpret_cast<void*>(alignedAddr);
}

void FreeListAllocator::Free(void* const ptr) noexcept
{
    assert(ptr != nullptr);

    // retrieve the header from the space we allocated prior to ptr
    AllocationHeader* header = reinterpret_cast<AllocationHeader*>(
        subPtr(ptr, sizeof(AllocationHeader)));

    // retrieve the real start of the allocation by moving backwards by the
    // amount needed for alignment
    std::uintptr_t blockStart =
        reinterpret_cast<std::uintptr_t>(ptr) - header->adjustment;
    std::size_t blockSize = header->size;
    std::uintptr_t blockEnd = blockStart + blockSize;

    FreeBlock* prevFreeBlock = nullptr;
    FreeBlock* freeBlock = m_freeBlocks;

    // find the first free block that starts after or is at the boundary of this
    // block
    while (freeBlock != nullptr)
    {
        if (reinterpret_cast<std::uintptr_t>(freeBlock) >= blockEnd)
        {
            break;
        }

        prevFreeBlock = freeBlock;
        freeBlock = freeBlock->next;
    }

    if (prevFreeBlock == nullptr)
    {
        // there was no free block that starts after the one we're freeing
        // so we can add it to the start of the list
        prevFreeBlock = reinterpret_cast<FreeBlock*>(blockStart);
        prevFreeBlock->size = blockSize;
        prevFreeBlock->next = m_freeBlocks;

        m_freeBlocks = prevFreeBlock;
    }
    else if (
        reinterpret_cast<std::uintptr_t>(prevFreeBlock) + prevFreeBlock->size ==
        blockStart)
    {
        // the block that starts after the one we're freeing, has a block before
        // it on the free list that ends right on the boundary starting our
        // block

        // this means that block that bounds ours can be merged into this one to
        // coalesce into a larger block
        prevFreeBlock->size += blockSize;
    }
    else
    {
        // the block that starts after the one we're freeing, does not have a
        // block before it that we can coalesce with.

        // so let's create a new block at the position we're freeing and insert
        // it into the free list

        FreeBlock* temp = reinterpret_cast<FreeBlock*>(blockStart);
        temp->size = blockSize;
        temp->next = prevFreeBlock->next;

        prevFreeBlock->next = temp;
        prevFreeBlock = temp;
    }

    if (reinterpret_cast<std::uintptr_t>(prevFreeBlock) + prevFreeBlock->size ==
        reinterpret_cast<std::uintptr_t>(prevFreeBlock->next))
    {
        // the new or merged block ends right on the next block on the free list
        // so we can merge them

        prevFreeBlock->size += prevFreeBlock->next->size;
        prevFreeBlock->next = prevFreeBlock->next->next;
    }

    --m_numAllocations;
    m_usedBytes -= blockSize;
}
}
