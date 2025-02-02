#include <ecs/ComponentSparseSet.hpp>

#include <gtest/gtest.h>

struct TestComponent
{
    float x{}, y{};
};

TEST(ComponentSparseSetTest, Push_EntityWithDefaultComponent)
{
    Zeus::ECS::ComponentSparseSet<TestComponent> sut;
    Zeus::ECS::Entity entity{ 0 };

    sut.Push(entity);

    auto empty = sut.Empty();
    auto capacity = sut.Capacity();
    auto size = sut.Size();
    auto contains = sut.Contains(entity);
    auto& actual = sut.Get(entity);

    EXPECT_FALSE(empty);
    EXPECT_EQ(capacity, 1);
    EXPECT_EQ(size, 1);
    EXPECT_TRUE(contains);

    EXPECT_EQ(actual.x, 0);
    EXPECT_EQ(actual.y, 0);
}

TEST(ComponentSparseSetTest, Emplace_EmptyRValueComponent)
{
    Zeus::ECS::ComponentSparseSet<TestComponent> sut;
    Zeus::ECS::Entity entity{ 0 };

    sut.Emplace<TestComponent>(entity, {});

    auto empty = sut.Empty();
    auto capacity = sut.Capacity();
    auto size = sut.Size();
    auto contains = sut.Contains(entity);
    auto& actual = sut.Get(entity);

    EXPECT_FALSE(empty);
    EXPECT_EQ(capacity, 1);
    EXPECT_EQ(size, 1);
    EXPECT_TRUE(contains);

    EXPECT_EQ(actual.x, 0);
    EXPECT_EQ(actual.y, 0);
}

TEST(ComponentSparseSetTest, Emplace_RValueComponent)
{
    Zeus::ECS::ComponentSparseSet<TestComponent> sut;
    Zeus::ECS::Entity entity{ 0 };

    sut.Emplace<TestComponent>(entity, { .x = 1, .y = 2 });

    auto empty = sut.Empty();
    auto capacity = sut.Capacity();
    auto size = sut.Size();
    auto contains = sut.Contains(entity);
    auto& actual = sut.Get(entity);

    EXPECT_FALSE(empty);
    EXPECT_EQ(capacity, 1);
    EXPECT_EQ(size, 1);
    EXPECT_TRUE(contains);

    EXPECT_EQ(actual.x, 1);
    EXPECT_EQ(actual.y, 2);
}

TEST(ComponentSparseSetTest, Emplace_RValueArguments)
{
    Zeus::ECS::ComponentSparseSet<TestComponent> sut;
    Zeus::ECS::Entity entity{ 0 };

    sut.Emplace(entity, 1, 2);

    auto empty = sut.Empty();
    auto capacity = sut.Capacity();
    auto size = sut.Size();
    auto contains = sut.Contains(entity);
    auto& actual = sut.Get(entity);

    EXPECT_FALSE(empty);
    EXPECT_EQ(capacity, 1);
    EXPECT_EQ(size, 1);
    EXPECT_TRUE(contains);

    EXPECT_EQ(actual.x, 1);
    EXPECT_EQ(actual.y, 2);
}

TEST(ComponentSparseSetTest, Emplace_LValueComponent)
{
    Zeus::ECS::ComponentSparseSet<TestComponent> sut;
    TestComponent component{ .x = 1, .y = 2 };
    Zeus::ECS::Entity entity{ 0 };

    sut.Emplace(entity, component);

    auto empty = sut.Empty();
    auto capacity = sut.Capacity();
    auto size = sut.Size();
    auto contains = sut.Contains(entity);
    auto& actual = sut.Get(entity);

    EXPECT_FALSE(empty);
    EXPECT_EQ(capacity, 1);
    EXPECT_EQ(size, 1);
    EXPECT_TRUE(contains);

    EXPECT_EQ(actual.x, 1);
    EXPECT_EQ(actual.y, 2);
}
