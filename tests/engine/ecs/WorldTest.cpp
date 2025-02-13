#include <ecs/Entity.hpp>
#include <ecs/World.hpp>

#include <gtest/gtest.h>

using namespace Zeus;

struct AComponent
{
    int number;
};

struct BComponent
{
    int number;
    const char* string;
};

TEST(WorldTest, Create_EmptyEntity_IsValid)
{
    ECS::World sut;

    ECS::Entity entity0 = sut.Create();
    ECS::Entity entity1 = sut.Create();

    auto contains0 = sut.IsValid(entity0);
    auto contains1 = sut.IsValid(entity1);
    auto contains2 = sut.IsValid(2);

    EXPECT_TRUE(contains0);
    EXPECT_TRUE(contains1);
    EXPECT_FALSE(contains2);
}

TEST(WorldTest, Destroy_EmptyEntity_IsValid)
{
    ECS::World sut;

    ECS::Entity entity0 = sut.Create();
    ECS::Entity entity1 = sut.Create();

    sut.Destroy(entity0);
    auto contains0 = sut.IsValid(entity0);
    auto contains1 = sut.IsValid(entity1);
    auto contains2 = sut.IsValid(2);

    EXPECT_FALSE(contains0);
    EXPECT_TRUE(contains1);
    EXPECT_FALSE(contains2);
}

TEST(WorldTest, Clear_EmptyEntity_Empty)
{
    ECS::World sut;

    ECS::Entity entity0 = sut.Create();
    ECS::Entity entity1 = sut.Create();

    sut.Clear();
    auto contains0 = sut.IsValid(entity0);
    auto contains1 = sut.IsValid(entity1);
    auto contains2 = sut.IsValid(2);

    EXPECT_FALSE(contains0);
    EXPECT_FALSE(contains1);
    EXPECT_FALSE(contains2);
}

TEST(WorldTest, Emplace_SingleNothing)
{
    ECS::World sut;
    ECS::Entity entity{ 0 };

    auto actual = sut.Emplace<AComponent>(entity);
    auto contains = sut.IsValid(entity);

    EXPECT_TRUE(contains);
    EXPECT_EQ(actual.number, 0);
}

TEST(WorldTest, Emplace_SingleEmptyComponent)
{
    ECS::World sut;
    ECS::Entity entity{ 0 };

    auto actual = sut.Emplace<AComponent>(entity, AComponent{});
    auto contains = sut.IsValid(entity);

    EXPECT_TRUE(contains);
    EXPECT_EQ(actual.number, 0);
}

TEST(WorldTest, Emplace_SingleValidComponent)
{
    ECS::World sut;
    ECS::Entity entity0{ 0 };
    ECS::Entity entity1{ 1 };
    ECS::Entity entity2{ 2 };

    auto actual0 = sut.Emplace<AComponent>(entity0, 42);
    auto actual1 = sut.Emplace<AComponent>(entity1, AComponent{ .number = 24 });
    auto actual2 = sut.Emplace<BComponent>(entity2, 1, "Test");
    auto contains0 = sut.IsValid(entity0);
    auto contains1 = sut.IsValid(entity1);
    auto contains2 = sut.IsValid(entity2);

    EXPECT_TRUE(contains0);
    EXPECT_TRUE(contains1);
    EXPECT_TRUE(contains2);
    EXPECT_EQ(actual0.number, 42);
    EXPECT_EQ(actual1.number, 24);
    EXPECT_EQ(actual2.number, 1);
    EXPECT_STREQ(actual2.string, "Test");
}

TEST(WorldTest, Emplace_MultipleComponent)
{
    ECS::World sut;
    ECS::Entity entity{ 0 };

    auto actual1 = sut.Emplace<AComponent>(entity, 42);
    auto actual2 = sut.Emplace<BComponent>(entity, 1, "Test");
    auto contains = sut.IsValid(entity);

    EXPECT_TRUE(contains);
    EXPECT_EQ(actual1.number, 42);
    EXPECT_EQ(actual2.number, 1);
    EXPECT_STREQ(actual2.string, "Test");
}

TEST(WorldTest, Patch_SingleComponent)
{
    ECS::World sut;
    ECS::Entity entity{ 0 };

    auto& component = sut.Emplace<BComponent>(entity, 1, "Test");
    auto& actual = sut.Patch<BComponent>(entity, [](BComponent& value) {
        value.number = 42;
        value.string = "Test2";
    });

    EXPECT_EQ(component.number, 42);
    EXPECT_STREQ(component.string, "Test2");
    EXPECT_EQ(actual.number, 42);
    EXPECT_STREQ(actual.string, "Test2");
}

TEST(WorldTest, Get_SingleComponent)
{
    ECS::World sut;
    ECS::Entity entity{ 0 };

    sut.Emplace<AComponent>(entity, 42);
    sut.Emplace<BComponent>(entity, 1, "Test");

    auto& actual1 = sut.Get<AComponent>(entity);
    auto& actual2 = sut.Get<BComponent>(entity);

    EXPECT_EQ(actual1.number, 42);

    EXPECT_EQ(actual2.number, 1);
    EXPECT_STREQ(actual2.string, "Test");
}

TEST(WorldTest, Get_MultipleComponent)
{
    ECS::World sut;
    ECS::Entity entity{ 0 };

    sut.Emplace<AComponent>(entity, 42);
    sut.Emplace<BComponent>(entity, 1, "Test");

    auto [actual1, actual2] = sut.Get<AComponent, BComponent>(entity);

    EXPECT_EQ(actual1.number, 42);

    EXPECT_EQ(actual2.number, 1);
    EXPECT_STREQ(actual2.string, "Test");
}

TEST(WorldTest, AnyOf_EmptyEntity_False)
{
    ECS::World sut;
    ECS::Entity entity = sut.Create();

    bool actual1 = sut.AnyOf<AComponent>(entity);
    bool actual2 = sut.AnyOf<AComponent, BComponent>(entity);

    EXPECT_FALSE(actual1);
    EXPECT_FALSE(actual2);
}

TEST(WorldTest, AllOf_EmptyEntity_False)
{
    ECS::World sut;
    ECS::Entity entity = sut.Create();

    bool actual1 = sut.AllOf<AComponent>(entity);
    bool actual2 = sut.AllOf<AComponent, BComponent>(entity);

    EXPECT_FALSE(actual1);
    EXPECT_FALSE(actual2);
}

TEST(WorldTest, AllOf_SingleComponent)
{
    ECS::World sut;
    ECS::Entity entity0{ 0 };
    ECS::Entity entity1{ 1 };

    sut.Emplace<AComponent>(entity0, 1);
    sut.Emplace<BComponent>(entity1, 1, "Test");

    bool actual1 = sut.AllOf<AComponent>(entity1);
    bool actual2 = sut.AllOf<BComponent>(entity1);

    EXPECT_FALSE(actual1);
    EXPECT_TRUE(actual2);
}

TEST(WorldTest, AllOf_MultipleComponent)
{
    ECS::World sut;
    ECS::Entity entity0{ 0 };
    ECS::Entity entity1{ 1 };

    sut.Emplace<AComponent>(entity0, 1);
    sut.Emplace<AComponent>(entity1, 42);
    sut.Emplace<BComponent>(entity1, 1, "Test");

    bool actual1 = sut.AllOf<float, BComponent>(entity1);
    bool actual2 = sut.AllOf<AComponent, BComponent>(entity1);

    EXPECT_FALSE(actual1);
    EXPECT_TRUE(actual2);
}

TEST(WorldTest, AnyOf_SingleComponent)
{
    ECS::World sut;
    ECS::Entity entity0{ 0 };
    ECS::Entity entity1{ 1 };

    sut.Emplace<AComponent>(entity0, 1);
    sut.Emplace<BComponent>(entity1, 1, "Test");

    bool actual1 = sut.AnyOf<AComponent>(entity1);
    bool actual2 = sut.AnyOf<BComponent>(entity1);

    EXPECT_FALSE(actual1);
    EXPECT_TRUE(actual2);
}

TEST(WorldTest, AnyOf_MultipleComponent)
{
    ECS::World sut;
    ECS::Entity entity0{ 0 };
    ECS::Entity entity1{ 1 };

    sut.Emplace<AComponent>(entity0, 1);
    sut.Emplace<AComponent>(entity1, 42);
    sut.Emplace<BComponent>(entity1, 1, "Test");

    bool actual0 = sut.AnyOf<float>(entity1);
    bool actual1 = sut.AnyOf<float, BComponent>(entity1);
    bool actual2 = sut.AnyOf<AComponent, BComponent>(entity1);

    EXPECT_FALSE(actual0);
    EXPECT_TRUE(actual1);
    EXPECT_TRUE(actual2);
}

TEST(WorldTest, Query)
{
    ECS::World sut;
    ECS::Entity entity = sut.Create();
    // sut.Emplace<AComponent>(entity, 1, "Test");

    // auto [a, b] = sut.Get<AComponent, BComponent>(entity);
    // auto actual = sut.Get<AComponent>(entity);

    /*EXPECT_EQ(a.number, 1);*/
    /*EXPECT_STREQ(actual.string, "Test");*/
    /*auto query = sut.Query<AComponent>();*/

    /*EXPECT_TRUE(false);*/
}

TEST(WorldTest, IsValid)
{
    ECS::World sut;
    ECS::Entity entity = sut.Create();

    bool actual1 = sut.IsValid(entity);
    bool actual2 = sut.IsValid(1);

    EXPECT_TRUE(actual1);
    EXPECT_FALSE(actual2);
}
