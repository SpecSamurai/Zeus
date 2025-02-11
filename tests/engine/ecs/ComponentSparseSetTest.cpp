#include <ecs/ComponentSparseSet.hpp>

#include <gtest/gtest.h>

struct TestComponent
{
    float x{}, y{};
};

TEST(ComponentSparseSetTest, Push_Entity_EntityHasEmptyComponent)
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
    Zeus::ECS::Entity entity1{ 0 };
    Zeus::ECS::Entity entity2{ 2 };

    sut.Emplace(entity1, 1, 2);
    sut.Emplace(entity2, 3, 4);

    auto empty = sut.Empty();
    auto capacity = sut.Capacity();
    auto size = sut.Size();
    auto contains1 = sut.Contains(entity1);
    auto contains2 = sut.Contains(entity2);
    auto& actual1 = sut.Get(entity1);
    auto& actual2 = sut.Get(entity2);

    EXPECT_FALSE(empty);
    EXPECT_EQ(capacity, 2);
    EXPECT_EQ(size, 2);
    EXPECT_TRUE(contains1);
    EXPECT_TRUE(contains2);

    EXPECT_EQ(actual1.x, 1);
    EXPECT_EQ(actual1.y, 2);
    EXPECT_EQ(actual2.x, 3);
    EXPECT_EQ(actual2.y, 4);
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

TEST(ComponentSparseSetTest, Patch_ModifyComponent)
{
    Zeus::ECS::ComponentSparseSet<TestComponent> sut;
    TestComponent component{ .x = 1, .y = 2 };
    Zeus::ECS::Entity entity{ 0 };
    sut.Emplace(entity, component);

    sut.Patch(entity, [](TestComponent& value) {
        value.x = 42;
        value.y = 24;
    });

    auto empty = sut.Empty();
    auto capacity = sut.Capacity();
    auto size = sut.Size();
    auto contains = sut.Contains(entity);
    auto& actual = sut.Get(entity);

    EXPECT_FALSE(empty);
    EXPECT_EQ(capacity, 1);
    EXPECT_EQ(size, 1);
    EXPECT_TRUE(contains);

    EXPECT_EQ(actual.x, 42);
    EXPECT_EQ(actual.y, 24);
}

TEST(ComponentSparseSetTest, Pop_EntityNotFound)
{
    Zeus::ECS::ComponentSparseSet<TestComponent> sut;
    TestComponent component{ .x = 1, .y = 2 };
    Zeus::ECS::Entity entity1{ 0 };
    Zeus::ECS::Entity entity2{ 1 };
    sut.Emplace(entity1, component);
    sut.Emplace(entity2, component);

    sut.Pop(entity1);

    auto empty = sut.Empty();
    auto capacity = sut.Capacity();
    auto size = sut.Size();
    auto contains1 = sut.Contains(entity1);
    auto contains2 = sut.Contains(entity2);
    auto& actual = sut.Get(entity2);

    EXPECT_FALSE(empty);
    EXPECT_EQ(capacity, 2);
    EXPECT_EQ(size, 1);
    EXPECT_FALSE(contains1);
    EXPECT_TRUE(contains2);

    EXPECT_EQ(actual.x, 1);
    EXPECT_EQ(actual.y, 2);
}
