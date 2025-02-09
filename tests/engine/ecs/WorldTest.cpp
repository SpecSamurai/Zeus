#include <ecs/Entity.hpp>
#include <ecs/World.hpp>

#include <gtest/gtest.h>

using namespace Zeus;

struct AComponent
{
    int number;
    const char* string;
};

struct BComponent
{
    int number;
};

TEST(WorldTest, Create_NewEntity)
{
    ECS::World sut;

    ECS::Entity entity0 = sut.Create();
    ECS::Entity entity1 = sut.Create();
    ECS::Entity entity2 = sut.Create();

    /*EXPECT_EQ(entity0, 0);*/
    /*EXPECT_EQ(entity1, 1);*/
    /*EXPECT_EQ(entity2, 2);*/
}

TEST(WorldTest, Emplace_Component)
{
    ECS::World sut;
    ECS::Entity entity = sut.Create();
    AComponent component{ 1, "Test" };

    auto actual = sut.Emplace(entity, component);

    EXPECT_EQ(actual.number, 1);
    EXPECT_STREQ(actual.string, "Test");
}

TEST(WorldTest, Emplace_Args)
{
    ECS::World sut;
    ECS::Entity entity = sut.Create();

    auto actual = sut.Emplace<AComponent>(entity, 1, "Test");

    EXPECT_EQ(actual.number, 1);
    EXPECT_STREQ(actual.string, "Test");
}

TEST(WorldTest, Get)
{
    ECS::World sut;
    ECS::Entity entity = sut.Create();
    sut.Emplace<AComponent>(entity, 1, "Test");

    auto [a, b] = sut.Get<AComponent, BComponent>(entity);
    auto actual = sut.Get<AComponent>(entity);

    /*EXPECT_EQ(a.number, 1);*/
    /*EXPECT_STREQ(actual.string, "Test");*/
}

TEST(WorldTest, Query)
{
    ECS::World sut;
    ECS::Entity entity = sut.Create();
    sut.Emplace<AComponent>(entity, 1, "Test");

    /*auto query = sut.Query<AComponent>();*/

    /*EXPECT_TRUE(false);*/
}

TEST(WorldTest, AnyOf_EmptyEntity_False)
{
    ECS::World sut;
    ECS::Entity entity = sut.Create();

    /*bool actual1 = sut.AnyOf<AComponent>(entity);*/
    /*bool actual2 = sut.AnyOf<AComponent, BComponent>(entity);*/

    /*EXPECT_FALSE(actual1);*/
    /*EXPECT_FALSE(actual2);*/
}

TEST(WorldTest, AnyOf_EntityWithComponent)
{
}

TEST(WorldTest, AllOf_EmptyEntity_False)
{
    ECS::World sut;
    ECS::Entity entity = sut.Create();

    /*bool actual1 = sut.AllOf<AComponent>(entity);*/
    /*bool actual2 = sut.AllOf<AComponent, BComponent>(entity);*/
    /**/
    /*EXPECT_FALSE(actual1);*/
    /*EXPECT_FALSE(actual2);*/
}

TEST(WorldTest, AllOf_EntityWithComponent_True)
{
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
