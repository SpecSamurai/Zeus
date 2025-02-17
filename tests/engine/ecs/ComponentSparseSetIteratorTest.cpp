#include <ecs/ComponentSparseSet.hpp>
#include <ecs/Entity.hpp>

#include <gtest/gtest.h>

struct TestIteratorComponent
{
    int a;
};

TEST(ComponentSparseSetIteratorTest, ForEach)
{
    TestIteratorComponent expected[]{ { 1 }, { 2 }, { 4 } };
    Zeus::ECS::ComponentSparseSet<TestIteratorComponent> sut;
    sut.Emplace(0, 1);
    sut.Emplace(1, 2);
    sut.Emplace(5, 4);

    int i{ 0 };
    for (const TestIteratorComponent& component : sut)
    {
        EXPECT_EQ(component.a, expected[i++].a);
    }
}

TEST(ComponentSparseSetIteratorTest, Begin_IncrementOperators)
{
    Zeus::ECS::ComponentSparseSet<TestIteratorComponent> sut;
    sut.Emplace(0, 1);
    sut.Emplace(1, 2);
    sut.Emplace(5, 4);
    sut.Emplace(6, 5);
    sut.Emplace(7, 6);

    auto actual = sut.begin();
    EXPECT_EQ(actual->a, 1);

    ++actual;
    EXPECT_EQ(actual->a, 2);

    EXPECT_EQ((actual++)->a, 2);
    EXPECT_EQ(actual->a, 4);

    EXPECT_EQ((actual + 1)->a, 5);
    EXPECT_EQ((1 + actual)->a, 5);

    actual += 2;
    EXPECT_EQ(actual->a, 6);

    EXPECT_EQ(actual + 1, sut.end());
}

TEST(ComponentSparseSetIteratorTest, End_DecrementOperators)
{
    Zeus::ECS::ComponentSparseSet<TestIteratorComponent> sut;
    sut.Emplace(0, 1);
    sut.Emplace(1, 2);
    sut.Emplace(5, 4);
    sut.Emplace(6, 5);
    sut.Emplace(7, 6);

    auto actual = sut.end() - 1;
    EXPECT_EQ(actual->a, 6);

    --actual;
    EXPECT_EQ(actual->a, 5);

    EXPECT_EQ((actual--)->a, 5);
    EXPECT_EQ(actual->a, 4);

    EXPECT_EQ((actual - 1)->a, 2);

    actual -= 2;
    EXPECT_EQ(actual->a, 1);
    EXPECT_EQ(actual, sut.begin());
}

TEST(ComponentSparseSetIteratorTest, SubtractIteratorsOperators_difference_type)
{
    Zeus::ECS::ComponentSparseSet<TestIteratorComponent> sut;
    sut.Emplace(0, 1);
    sut.Emplace(1, 2);
    sut.Emplace(5, 4);
    sut.Emplace(6, 5);
    sut.Emplace(7, 6);

    auto actual1 = sut.begin() + 1;
    auto actual2 = sut.begin() + 3;

    EXPECT_EQ(actual1 - actual2, -2);
    EXPECT_EQ(actual2 - actual1, 2);
}

TEST(ComponentSparseSetIteratorTest, BoolOperators)
{
    Zeus::ECS::ComponentSparseSet<TestIteratorComponent> sut;
    sut.Emplace(0, 1);
    sut.Emplace(1, 2);
    sut.Emplace(5, 4);

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

TEST(ComponentSparseSetIteratorTest, IndexOperator)
{
    Zeus::ECS::ComponentSparseSet<TestIteratorComponent> sut;
    sut.Emplace(0, 1);
    sut.Emplace(1, 2);
    sut.Emplace(5, 4);

    auto actual = sut.begin();

    EXPECT_EQ(actual[0].a, 1);
    EXPECT_EQ(actual[1].a, 2);
    EXPECT_EQ(actual[2].a, 4);
}
