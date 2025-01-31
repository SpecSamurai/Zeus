#include <ecs/Entity.hpp>
#include <ecs/SparseSet.hpp>

#include <gtest/gtest.h>

TEST(SparseSetTest, Constructor_Default_Empty)
{
    Zeus::ECS::SparseSet sut;

    auto isEmpty = sut.Empty();
    auto size = sut.Size();
    auto capacity = sut.Capacity();

    EXPECT_TRUE(isEmpty);
    EXPECT_EQ(size, 0);
    EXPECT_EQ(capacity, 0);
}

TEST(SparseSetTest, Constructor_MaxEntity_ReservedCapacity)
{
    Zeus::ECS::SparseSet sut(10);

    auto isEmpty = sut.Empty();
    auto size = sut.Size();
    auto capacity = sut.Capacity();

    assert(isEmpty);
    assert(size == 0);
    assert(capacity == 10);
}

TEST(SparseSetTest, Reserved_EmptySet)
{
    Zeus::ECS::SparseSet sut;
    sut.Reserve(10);

    auto isEmpty = sut.Empty();
    auto size = sut.Size();
    auto capacity = sut.Capacity();

    assert(isEmpty);
    assert(size == 0);
    assert(capacity == 10);
}

TEST(SparseSetTest, Push_EmptySet)
{
    Zeus::ECS::SparseSet sut;
    Zeus::ECS::Entity entity{ 1 };

    sut.Push(entity);

    auto isEmpty = sut.Empty();
    auto size = sut.Size();
    auto capacity = sut.Capacity();

    assert(!isEmpty);
    assert(size == 1);
    assert(capacity == 1);
}

TEST(SparseSetTest, Push_Reserved)
{
    Zeus::ECS::SparseSet sut;
    sut.Reserve(10);

    sut.Push(1);

    auto isEmpty = sut.Empty();
    auto size = sut.Size();
    auto capacity = sut.Capacity();

    assert(!isEmpty);
    assert(size == 1);
    assert(capacity == 10);
}

TEST(SparseSetTest, Contains_EmptySet)
{
    Zeus::ECS::SparseSet sut;

    auto actual1 = sut.Contains(0);
    auto actual2 = sut.Contains(1);
    auto actual3 = sut.Contains(2);

    assert(!actual1);
    assert(!actual2);
    assert(!actual3);
}

TEST(SparseSetTest, Contains_Reserved_EmptySet)
{
    Zeus::ECS::SparseSet sut(100);

    auto actual1 = sut.Contains(0);
    auto actual2 = sut.Contains(1);
    auto actual3 = sut.Contains(2);

    assert(!actual1);
    assert(!actual2);
    assert(!actual3);
}

TEST(SparseSetTest, Contains_Multiple)
{
    Zeus::ECS::SparseSet sut;

    sut.Push(0);
    sut.Push(1);
    sut.Push(5);

    auto isEmpty = sut.Empty();
    auto size = sut.Size();
    auto capacity = sut.Capacity();
    auto data = sut.Data();

    assert(!isEmpty);
    assert(size == 3);
    assert(capacity == 3);

    assert(sut.Contains(0));
    assert(sut.Contains(1));
    assert(sut.Contains(5));
    assert(!sut.Contains(2));

    assert(data[0] == 0);
    assert(data[1] == 1);
    assert(data[2] == 5);
}

TEST(SparseSetTest, Index)
{
    Zeus::ECS::SparseSet sut;

    sut.Push(0);
    sut.Push(1);
    sut.Push(5);

    assert(sut.Index(0) == 0);
    assert(sut.Index(1) == 1);
    assert(sut.Index(5) == 2);
}

TEST(SparseSetTest, Clear)
{
    Zeus::ECS::SparseSet sut;
    sut.Push(0);
    sut.Push(1);
    sut.Push(5);

    sut.Clear();

    auto isEmpty = sut.Empty();
    auto size = sut.Size();

    assert(isEmpty);
    assert(size == 0);
    assert(!sut.Contains(0));
    assert(!sut.Contains(1));
    assert(!sut.Contains(2));
    assert(!sut.Contains(5));
}

TEST(SparseSetTest, Pop)
{
    Zeus::ECS::SparseSet sut;
    sut.Push(0);
    sut.Push(1);
    sut.Push(5);

    sut.Pop(1);

    assert(!sut.Contains(1));
    assert(sut.Size() == 2);
    assert(!sut.Empty());

    sut.Pop(0);

    assert(!sut.Contains(0));
    assert(sut.Size() == 1);
    assert(!sut.Empty());

    sut.Pop(5);

    assert(!sut.Contains(5));
    assert(sut.Size() == 0);
    assert(sut.Empty());
}
