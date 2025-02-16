#include <ecs/Entity.hpp>
#include <ecs/SparseSet.hpp>

#include <gtest/gtest.h>

TEST(SparseSetIteratorTest, ForEach)
{
    Zeus::ECS::Entity expected[]{ 0, 1, 5 };
    Zeus::ECS::SparseSet sut;
    sut.Push(0);
    sut.Push(1);
    sut.Push(5);

    int i{ 0 };
    for (const Zeus::ECS::Entity& entity : sut)
    {
        EXPECT_EQ(entity, expected[i++]);
    }
}

TEST(SparseSetIteratorTest, Begin_IncrementOperators)
{
    Zeus::ECS::SparseSet sut;
    sut.Push(0);
    sut.Push(1);
    sut.Push(5);
    sut.Push(6);
    sut.Push(7);

    auto actual = sut.begin();
    EXPECT_EQ(*actual, 0);

    ++actual;
    EXPECT_EQ(*actual, 1);

    EXPECT_EQ(*(actual++), 1);
    EXPECT_EQ(*actual, 5);

    EXPECT_EQ(*(actual + 1), 6);
    EXPECT_EQ(*(1 + actual), 6);

    actual += 2;
    EXPECT_EQ(*actual, 7);

    EXPECT_EQ((actual + 1), sut.end());
}

TEST(SparseSetIteratorTest, End_DecrementOperators)
{
    Zeus::ECS::SparseSet sut;
    sut.Push(0);
    sut.Push(1);
    sut.Push(5);
    sut.Push(6);
    sut.Push(7);

    auto actual = sut.end() - 1;
    EXPECT_EQ(*actual, 7);

    --actual;
    EXPECT_EQ(*actual, 6);

    EXPECT_EQ(*(actual--), 6);
    EXPECT_EQ(*actual, 5);

    EXPECT_EQ(*(actual - 1), 1);

    actual -= 2;
    EXPECT_EQ(*actual, 0);
    EXPECT_EQ(actual, sut.begin());
}

TEST(SparseSetIteratorTest, SubtractIteratorsOperators_difference_type)
{
    Zeus::ECS::SparseSet sut;
    sut.Push(0);
    sut.Push(1);
    sut.Push(5);
    sut.Push(6);
    sut.Push(7);

    auto actual1 = sut.begin() + 1;
    auto actual2 = sut.begin() + 3;

    EXPECT_EQ(actual1 - actual2, -2);
    EXPECT_EQ(actual2 - actual1, 2);
}

TEST(SparseSetIteratorTest, BoolOperators)
{
    Zeus::ECS::SparseSet sut;
    sut.Push(0);
    sut.Push(1);
    sut.Push(5);

    auto actual1 = sut.begin();
    auto actual2 = sut.begin() + 1;

    EXPECT_TRUE(actual1 != actual2);
    EXPECT_TRUE(actual2 > actual1);
    EXPECT_TRUE(actual2 >= actual1);
    EXPECT_TRUE(actual1 >= actual1);
    EXPECT_TRUE(actual2 >= actual2);

    EXPECT_TRUE(actual1 < actual2);
    EXPECT_TRUE(actual1 <= actual2);
    EXPECT_TRUE(actual1 <= actual1);
    EXPECT_TRUE(actual2 <= actual2);
}

TEST(SparseSetIteratorTest, IndexOperator)
{
    Zeus::ECS::SparseSet sut;
    sut.Push(0);
    sut.Push(1);
    sut.Push(5);

    auto actual = sut.begin();

    EXPECT_EQ(actual[0], 0);
    EXPECT_EQ(actual[1], 1);
    EXPECT_EQ(actual[2], 5);
}
