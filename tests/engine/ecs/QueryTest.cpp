#include <ecs/Entity.hpp>
#include <ecs/Query.hpp>
#include <ecs/Registry.hpp>

#include <gtest/gtest.h>

using namespace Zeus;

struct TestComponent1
{
    float value;
};

struct TestComponent2
{
    int value;
    const char* text;
};

TEST(QueryTest, QueryIterator_SingleComponent_Loop)
{
    ECS::Registry sut;
    ECS::Entity entity0{ 0 };
    ECS::Entity entity1{ 1 };

    sut.Emplace<TestComponent1>(entity0, 1);
    sut.Emplace<TestComponent1>(entity1, 2);
    sut.Emplace<TestComponent2>(entity1, 42, "Test");

    auto query = sut.QueryAll<TestComponent1>();

    for (auto value : query)
    {
        EXPECT_TRUE(value.value == 1 || value.value == 2);
    }

    sut.Emplace<TestComponent1>(2, 3);
    sut.Emplace<TestComponent1>(3, 4);

    for (auto value : query)
    {
        EXPECT_TRUE(
            value.value == 1 || value.value == 2 || value.value == 3 ||
            value.value == 4);
    }
}

TEST(QueryTest, QueryIterator_SingleComponentLoopReference_EditComponents)
{
    ECS::Registry sut;
    ECS::Entity entity0{ 0 };
    ECS::Entity entity1{ 1 };

    sut.Emplace<TestComponent1>(entity0, 1);
    sut.Emplace<TestComponent1>(entity1, 2);
    sut.Emplace<TestComponent2>(entity1, 42, "Test");

    auto query = sut.QueryAll<TestComponent1>();

    for (auto& value : query)
    {
        value.value = 3;
    }

    for (auto value : query)
    {
        EXPECT_TRUE(value.value == 3);
    }

    auto actual1 = sut.Get<TestComponent1>(entity0);
    auto actual2 = sut.Get<TestComponent1>(entity1);

    EXPECT_EQ(actual1.value, 3);
    EXPECT_EQ(actual2.value, 3);
}

TEST(QueryTest, QueryIterator_Loop_MultipleComponents)
{
    ECS::Registry sut;
    ECS::Entity entity0{ 0 };
    ECS::Entity entity1{ 1 };
    ECS::Entity entity2{ 2 };
    ECS::Entity entity3{ 3 };

    sut.Emplace<TestComponent1>(entity0, 1);
    sut.Emplace<TestComponent2>(entity0, 2, "Test");
    sut.Emplace<TestComponent1>(entity1, 42);

    auto query = sut.QueryAll<TestComponent1, TestComponent2>();

    for (auto [a, b] : query)
    {
        EXPECT_EQ(a.value, 1);

        EXPECT_EQ(b.value, 2);
        EXPECT_STREQ(b.text, "Test");

        a.value = 41;
        b.value = 42;
        b.text = "Test42";
    }

    sut.Emplace<TestComponent2>(entity2, 3, "Test3");
    sut.Emplace<TestComponent2>(entity3, 4, "Test4");

    auto actual = query.begin();

    auto [cmpA, cmpB] = *actual;

    EXPECT_EQ(cmpA.value, 41);

    EXPECT_EQ(cmpB.value, 42);
    EXPECT_STREQ(cmpB.text, "Test42");
}

TEST(QueryTest, QueryIterator_IncrementOperator_SingleComponent)
{
    ECS::Registry sut;
    ECS::Entity entity0{ 0 };
    ECS::Entity entity1{ 1 };

    sut.Emplace<TestComponent2>(entity0, 1, "Test1");
    sut.Emplace<TestComponent2>(entity1, 2, "Test2");

    auto query = sut.QueryAll<TestComponent2>();
    auto actual = query.begin();

    auto value1 = *actual;
    EXPECT_EQ(value1.value, 1);
    EXPECT_STREQ(value1.text, "Test1");

    ++actual;
    auto value2 = *actual;
    EXPECT_EQ(value2.value, 2);
    EXPECT_STREQ(value2.text, "Test2");

    ++actual;
    EXPECT_EQ(actual, query.end());
}

TEST(
    QueryTest,
    QueryIterator_IncrementOperatorReference_EditComponents_SingleComponent)
{
    ECS::Registry sut;
    ECS::Entity entity0{ 0 };
    ECS::Entity entity1{ 1 };

    sut.Emplace<TestComponent2>(entity0, 1, "Test1");
    sut.Emplace<TestComponent2>(entity1, 2, "Test2");

    auto query = sut.QueryAll<TestComponent2>();
    auto actual = query.begin();

    auto& value1 = *actual;
    EXPECT_EQ(value1.value, 1);
    EXPECT_STREQ(value1.text, "Test1");

    value1.value = 10;
    value1.text = "Test10";

    EXPECT_EQ((*actual).value, 10);
    EXPECT_STREQ((*actual).text, "Test10");

    ++actual;
    auto& value2 = *actual;
    value2.value = 20;
    value2.text = "Test20";
    EXPECT_EQ((*actual).value, 20);
    EXPECT_STREQ((*actual).text, "Test20");

    ++actual;
    EXPECT_EQ(actual, query.end());
}
