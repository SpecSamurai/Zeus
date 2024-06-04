#include <memory/AllocationHeader.hpp>
#include <memory/FreeListAllocator.hpp>

#include "gtest/gtest.h"

#include <cstdlib>
#include <limits>

template <typename T>
static void Allocate_PrimitiveTypes_testPrimitiveType(
    Zeus::FreeListAllocator& sut)
{
    auto actualVoid{ sut.Allocate(sizeof(T), alignof(T)) };
    auto actualPtr{ reinterpret_cast<T*>(actualVoid) };

    auto limits{ std::numeric_limits<T>() };

    *actualPtr = limits.max();
    EXPECT_EQ(*actualPtr, limits.max());

    *actualPtr = limits.min();
    EXPECT_EQ(*actualPtr, limits.min());
}

TEST(FreeListAllocatorTest, Allocate_PrimitiveTypes)
{
    // Header 8
    // Header 8
    // Long Long 8
    // Header 8
    // Header 8
    // Freeblock 4 float 4
    // Header 8
    // Header 8
    // Freeblock 4 int 4
    // Header 8
    // Header 8
    // Freeblock 7 bool 1
    // Header 8
    // Header 8
    // Freeblock 4 bool 4

    auto maxSize{ 15u * sizeof(std::uintptr_t) };
    void* memStart{ std::malloc(maxSize) };

    auto sut{ Zeus::FreeListAllocator(maxSize, memStart) };

    Allocate_PrimitiveTypes_testPrimitiveType<long long>(sut);
    Allocate_PrimitiveTypes_testPrimitiveType<float>(sut);
    Allocate_PrimitiveTypes_testPrimitiveType<int>(sut);
    Allocate_PrimitiveTypes_testPrimitiveType<bool>(sut);
    Allocate_PrimitiveTypes_testPrimitiveType<int>(sut);

    EXPECT_EQ(sut.GetSize(), maxSize);
    EXPECT_EQ(sut.GetStart(), memStart);
    EXPECT_EQ(sut.GetUsedBytes(), maxSize);
    EXPECT_EQ(sut.GetNumAllocations(), 5);

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

TEST(FreeListAllocatorTest, Allocate_ComplexTypes)
{
    int* testPtr{ new int };
    *testPtr = 42;

    auto maxSize{ 200u };
    void* memStart{ std::malloc(maxSize) };

    auto sut{ Zeus::FreeListAllocator(maxSize, memStart) };

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

    // EXPECT_EQ(sut.GetSize(), maxSize);
    EXPECT_EQ(sut.GetStart(), memStart);
    // EXPECT_EQ(sut.GetUsedBytes(), maxSize);
    EXPECT_EQ(sut.GetNumAllocations(), 3);

    delete testPtr;
    std::free(memStart);
}

TEST(FreeListAllocatorTest, Free_First)
{
    auto maxSize{ 100u };
    void* memStart{ std::malloc(maxSize) };

    auto sut{ Zeus::FreeListAllocator(maxSize, memStart) };

    auto first = sut.Allocate(sizeof(TestClassA), alignof(TestClassA));
    auto second = reinterpret_cast<TestClassA*>(
        sut.Allocate(sizeof(TestClassA), alignof(TestClassA)));
    second->ptr = reinterpret_cast<int*>(42u);
    second->int32A = 42;

    sut.Free(first);

    EXPECT_EQ(second->ptr, reinterpret_cast<int*>(42u));
    EXPECT_EQ(second->int32A, 42);
    EXPECT_EQ(
        sut.GetUsedBytes(),
        sizeof(TestClassA) + sizeof(Zeus::AllocationHeader));
    EXPECT_EQ(sut.GetNumAllocations(), 1);

    std::free(memStart);
}
