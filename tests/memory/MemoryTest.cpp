#include <memory/memory.hpp>

#include "gtest/gtest.h"

#include <cstddef>
#include <cstdint>
#include <iterator>

TEST(MemoryTest, getMovedPtr_NullPtr_MovedByOffset)
{
    auto actual{ Zeus::addPtr(nullptr, 42u) };
    auto actualOffset{ reinterpret_cast<std::uintptr_t>(actual) };

    EXPECT_EQ(actualOffset, 42u);
}

TEST(MemoryTest, alignForwardAdjustment_AlignedPtrs_NoAdjustment)
{
    int testAddresses[]{ 0, 8, 16, 24, 32 };

    for (int addr : testAddresses)
    {
        void* testPtr{ reinterpret_cast<void*>(addr) };

        auto actual{ Zeus::alignForwardAdjustment(testPtr, 8u) };

        EXPECT_EQ(actual, 0) << "Test address: " << addr;
    }
}

TEST(MemoryTest, alignForwardAdjustment_UnalignedPtrs_Adjustment)
{
    int testAddresses[]{ 2, 4, 6, 10 };
    int expectedAdjustment[]{ 6, 4, 2, 6 };

    for (std::size_t i{ 0 }; i < std::size(testAddresses); ++i)
    {
        void* testPtr{ reinterpret_cast<void*>(testAddresses[i]) };

        auto actual{ Zeus::alignForwardAdjustment(testPtr, 8u) };

        EXPECT_EQ(actual, expectedAdjustment[i])
            << "Test address: " << testAddresses[i];
    }
}
