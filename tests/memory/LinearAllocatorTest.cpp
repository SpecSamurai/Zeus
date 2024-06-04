#include <memory/LinearAllocator.hpp>

#include "gtest/gtest.h"
#include <cstdlib>
#include <limits>

TEST(LinearAllocatorTest, Getters)
{
    void* testPtr{ reinterpret_cast<void*>(1337u) };
    auto sut{ Zeus::LinearAllocator{ 42u, testPtr } };

    EXPECT_EQ(sut.GetSize(), 42u);
    EXPECT_EQ(sut.GetStart(), testPtr);
    EXPECT_EQ(sut.GetCurrent(), testPtr);
    EXPECT_EQ(sut.GetUsedBytes(), 0);
    EXPECT_EQ(sut.GetNumAllocations(), 0);
}

template <typename T>
static void Allocate_PrimitiveTypes_testPrimitiveType(
    Zeus::LinearAllocator& sut)
{
    auto actualVoid{ sut.Allocate(sizeof(T), alignof(T)) };
    auto actualPtr{ reinterpret_cast<T*>(actualVoid) };

    auto limits{ std::numeric_limits<T>() };

    *actualPtr = limits.max();
    EXPECT_EQ(*actualPtr, limits.max());

    *actualPtr = limits.min();
    EXPECT_EQ(*actualPtr, limits.min());
}

TEST(LinearAllocatorTest, Allocate_PrimitiveTypes)
{
    auto maxSize{ sizeof(bool) + sizeof(int) + sizeof(float) +
                  sizeof(long long) };
    void* memStart{ std::malloc(maxSize) };

    auto sut{ Zeus::LinearAllocator(maxSize, memStart) };

    Allocate_PrimitiveTypes_testPrimitiveType<long long>(sut);
    Allocate_PrimitiveTypes_testPrimitiveType<float>(sut);
    Allocate_PrimitiveTypes_testPrimitiveType<int>(sut);
    Allocate_PrimitiveTypes_testPrimitiveType<bool>(sut);

    EXPECT_EQ(sut.GetSize(), maxSize);
    EXPECT_EQ(sut.GetStart(), memStart);
    EXPECT_EQ(sut.GetUsedBytes(), maxSize);
    EXPECT_EQ(sut.GetNumAllocations(), 4);

    std::free(memStart);
}

class TestClassA
{
public:
    int* ptr;
    int32_t int32A;
};

class TestClassB : public TestClassA
{
public:
    int32_t int32B;
};

TEST(LinearAllocatorTest, Allocate_ComplexTypes)
{
    int* testPtr{ new int };
    *testPtr = 42;

    auto maxSize{ sizeof(TestClassA) + 2 * sizeof(TestClassB) };
    void* memStart{ std::malloc(maxSize) };

    auto sut{ Zeus::LinearAllocator(maxSize, memStart) };

    auto classAPtr{ reinterpret_cast<TestClassA*>(
        sut.Allocate(sizeof(TestClassA), alignof(TestClassA))) };
    classAPtr->ptr = testPtr;
    classAPtr->int32A = 1;

    auto classB1Ptr{ reinterpret_cast<TestClassB*>(
        sut.Allocate(sizeof(TestClassB), alignof(TestClassB))) };
    classB1Ptr->ptr = testPtr;
    classB1Ptr->int32A = 2;
    classB1Ptr->int32B = 3;

    auto classB2Ptr{ reinterpret_cast<TestClassB*>(
        sut.Allocate(sizeof(TestClassB), alignof(TestClassB))) };
    classB2Ptr->ptr = testPtr;
    classB2Ptr->int32A = 4;
    classB2Ptr->int32B = 5;

    EXPECT_EQ(classAPtr->ptr, testPtr);
    EXPECT_EQ(classAPtr->int32A, 1);

    EXPECT_EQ(classB1Ptr->ptr, testPtr);
    EXPECT_EQ(classB1Ptr->int32A, 2);
    EXPECT_EQ(classB1Ptr->int32B, 3);

    EXPECT_EQ(classB2Ptr->ptr, testPtr);
    EXPECT_EQ(classB2Ptr->int32A, 4);
    EXPECT_EQ(classB2Ptr->int32B, 5);

    EXPECT_EQ(sut.GetSize(), maxSize);
    EXPECT_EQ(sut.GetStart(), memStart);
    EXPECT_EQ(sut.GetUsedBytes(), maxSize);
    EXPECT_EQ(sut.GetNumAllocations(), 3);

    delete testPtr;
    std::free(memStart);
}

TEST(LinearAllocatorTest, Clean_)
{
    auto maxSize{ sizeof(TestClassA) };
    void* memStart{ std::malloc(maxSize) };

    auto sut{ Zeus::LinearAllocator(maxSize, memStart) };

    sut.Allocate(sizeof(TestClassA), alignof(TestClassA));
    sut.Clear();

    EXPECT_EQ(sut.GetSize(), maxSize);
    EXPECT_EQ(sut.GetStart(), memStart);
    EXPECT_EQ(sut.GetCurrent(), memStart);
    EXPECT_EQ(sut.GetUsedBytes(), 0);
    EXPECT_EQ(sut.GetNumAllocations(), 0);

    std::free(memStart);
}

TEST(LinearAllocatorTest, Rewind_LastAllocation)
{
    auto maxSize{ 2 * sizeof(TestClassA) };
    void* memStart{ std::malloc(maxSize) };

    auto sut{ Zeus::LinearAllocator(maxSize, memStart) };
    sut.Allocate(sizeof(TestClassA), alignof(TestClassA));
    auto actual{ sut.Allocate(sizeof(TestClassA), alignof(TestClassA)) };

    sut.Rewind(actual);

    EXPECT_EQ(sut.GetSize(), maxSize);
    EXPECT_EQ(sut.GetStart(), memStart);
    EXPECT_EQ(sut.GetCurrent(), actual);
    EXPECT_EQ(sut.GetUsedBytes(), sizeof(TestClassA));
    EXPECT_EQ(sut.GetNumAllocations(), 2);

    std::free(memStart);
}

TEST(LinearAllocatorTest, Rewind_AllAllocation)
{
    auto maxSize{ 2 * sizeof(TestClassA) };
    void* memStart{ std::malloc(maxSize) };

    auto sut{ Zeus::LinearAllocator(maxSize, memStart) };

    auto actual{ sut.Allocate(sizeof(TestClassA), alignof(TestClassA)) };
    sut.Allocate(sizeof(TestClassA), alignof(TestClassA));

    sut.Rewind(actual);

    EXPECT_EQ(sut.GetSize(), maxSize);
    EXPECT_EQ(sut.GetStart(), actual);
    EXPECT_EQ(sut.GetCurrent(), actual);
    EXPECT_EQ(sut.GetUsedBytes(), 0);
    EXPECT_EQ(sut.GetNumAllocations(), 2);

    std::free(memStart);
}
