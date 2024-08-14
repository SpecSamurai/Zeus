#include <memory/memory.hpp>

#include "gtest/gtest.h"

#include <cstddef>
#include <cstdint>
#include <iterator>

TEST(MemoryTest, align_ZeroAlignment)
{
    std::size_t size1{ 0 };
    std::size_t size2{ 1 };
    std::size_t size3{ 2 };

    auto actual1{ Zeus::align(size1, 0) };
    auto actual2{ Zeus::align(size2, 0) };
    auto actual3{ Zeus::align(size3, 0) };

    EXPECT_EQ(actual1, 0);
    EXPECT_EQ(actual2, 0);
    EXPECT_EQ(actual3, 0);
}

TEST(MemoryTest, align_AlignmentPowerOfTwo)
{
    std::size_t size1{ 0 };
    std::size_t size2{ 1 };
    std::size_t size3{ 2 };
    std::size_t size4{ 3 };
    std::size_t size5{ 4 };
    std::size_t size6{ 5 };

    auto actual1{ Zeus::align(size1, 2) };
    auto actual2{ Zeus::align(size2, 4) };
    auto actual3{ Zeus::align(size3, 8) };
    auto actual4{ Zeus::align(size4, 16) };
    auto actual5{ Zeus::align(size5, 32) };
    auto actual6{ Zeus::align(size6, 64) };

    EXPECT_EQ(actual1, 0);
    EXPECT_EQ(actual2, 4);
    EXPECT_EQ(actual3, 8);
    EXPECT_EQ(actual4, 16);
    EXPECT_EQ(actual5, 32);
    EXPECT_EQ(actual6, 64);
}

TEST(MemoryTest, align_SizeGreaterOrEqualAlignment)
{
    std::size_t size{ 10 };

    auto actual1{ Zeus::align(size, 2) };
    auto actual2{ Zeus::align(size, 4) };
    auto actual3{ Zeus::align(size, 8) };
    auto actual4{ Zeus::align(size, 16) };

    EXPECT_EQ(actual1, 10);
    EXPECT_EQ(actual2, 12);
    EXPECT_EQ(actual3, 16);
    EXPECT_EQ(actual4, 16);
}

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
