#include <memory/memory.hpp>

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <iterator>

TEST(MemoryTest, isPowerOf2_True)
{
    EXPECT_TRUE(Zeus::isPowerOf2(0));
    EXPECT_TRUE(Zeus::isPowerOf2(1));
    EXPECT_TRUE(Zeus::isPowerOf2(2));
    EXPECT_TRUE(Zeus::isPowerOf2(4));
    EXPECT_TRUE(Zeus::isPowerOf2(8));
    EXPECT_TRUE(Zeus::isPowerOf2(128));
}

TEST(MemoryTest, isPowerOf2_False)
{
    EXPECT_FALSE(Zeus::isPowerOf2(3));
    EXPECT_FALSE(Zeus::isPowerOf2(10));
    EXPECT_FALSE(Zeus::isPowerOf2(20));
    EXPECT_FALSE(Zeus::isPowerOf2(22));
}

TEST(MemoryTest, alignUp_ZeroAlignment)
{
    auto actual1{ Zeus::alignUp(0, 0) };
    auto actual2{ Zeus::alignUp(1, 0) };
    auto actual3{ Zeus::alignUp(2, 0) };

    EXPECT_EQ(actual1, 0);
    EXPECT_EQ(actual2, 0);
    EXPECT_EQ(actual3, 0);
}

TEST(MemoryTest, alignUp_ValueLesserThanAlignment)
{
    auto actual1{ Zeus::alignUp(0, 2) };
    auto actual2{ Zeus::alignUp(1, 4) };
    auto actual3{ Zeus::alignUp(2, 8) };
    auto actual4{ Zeus::alignUp(3, 16) };
    auto actual5{ Zeus::alignUp(4, 32) };
    auto actual6{ Zeus::alignUp(5, 64) };

    EXPECT_EQ(actual1, 0);
    EXPECT_EQ(actual2, 4);
    EXPECT_EQ(actual3, 8);
    EXPECT_EQ(actual4, 16);
    EXPECT_EQ(actual5, 32);
    EXPECT_EQ(actual6, 64);
}

TEST(MemoryTest, alignUp_ValueGreaterOrEqualAlignment)
{
    auto actual1{ Zeus::alignUp(10, 2) };
    auto actual2{ Zeus::alignUp(10, 4) };
    auto actual3{ Zeus::alignUp(10, 8) };
    auto actual4{ Zeus::alignUp(10, 16) };

    EXPECT_EQ(actual1, 10);
    EXPECT_EQ(actual2, 12);
    EXPECT_EQ(actual3, 16);
    EXPECT_EQ(actual4, 16);
}

TEST(MemoryTest, alignDown_ZeroAlignment)
{
    auto actual1{ Zeus::alignDown(0, 0) };
    auto actual2{ Zeus::alignDown(1, 0) };
    auto actual3{ Zeus::alignDown(2, 0) };

    EXPECT_EQ(actual1, 0);
    EXPECT_EQ(actual2, 0);
    EXPECT_EQ(actual3, 0);
}

TEST(MemoryTest, alignDown_ValueLesserThanAlignment)
{
    auto actual1{ Zeus::alignDown(0, 2) };
    auto actual2{ Zeus::alignDown(3, 4) };
    auto actual3{ Zeus::alignDown(5, 8) };
    auto actual4{ Zeus::alignDown(11, 16) };
    auto actual5{ Zeus::alignDown(17, 32) };
    auto actual6{ Zeus::alignDown(5, 64) };

    EXPECT_EQ(actual1, 0);
    EXPECT_EQ(actual2, 0);
    EXPECT_EQ(actual3, 0);
    EXPECT_EQ(actual4, 0);
    EXPECT_EQ(actual5, 0);
    EXPECT_EQ(actual6, 0);
}

TEST(MemoryTest, alignDown_ValueGreaterOrEqualAlignment)
{
    auto actual1{ Zeus::alignDown(10, 2) };
    auto actual2{ Zeus::alignDown(10, 4) };
    auto actual3{ Zeus::alignDown(10, 8) };
    auto actual4{ Zeus::alignDown(10, 16) };

    EXPECT_EQ(actual1, 10);
    EXPECT_EQ(actual2, 8);
    EXPECT_EQ(actual3, 8);
    EXPECT_EQ(actual4, 0);
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
