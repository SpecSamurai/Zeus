#include <ecs/Entity.hpp>
#include <ecs/Registry.hpp>

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

TEST(RegistryTest, Create_EmptyEntity_IsValid)
{
    ECS::Registry sut;

    ECS::Entity entity0 = sut.Create();
    ECS::Entity entity1 = sut.Create();

    auto contains0 = sut.IsValid(entity0);
    auto contains1 = sut.IsValid(entity1);
    auto contains2 = sut.IsValid(2);

    EXPECT_TRUE(contains0);
    EXPECT_TRUE(contains1);
    EXPECT_FALSE(contains2);
}

TEST(RegistryTest, Destroy_EmptyEntity_IsValid)
{
    ECS::Registry sut;

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

TEST(RegistryTest, Clear_EmptyEntity_Empty)
{
    ECS::Registry sut;

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

TEST(RegistryTest, Emplace_SingleNothing)
{
    ECS::Registry sut;
    ECS::Entity entity{ 0 };

    auto actual = sut.Emplace<AComponent>(entity);
    auto contains = sut.IsValid(entity);

    EXPECT_TRUE(contains);
    EXPECT_EQ(actual.number, 0);
}

TEST(RegistryTest, Emplace_SingleEmptyComponent)
{
    ECS::Registry sut;
    ECS::Entity entity{ 0 };

    auto actual = sut.Emplace<AComponent>(entity, AComponent{});
    auto contains = sut.IsValid(entity);

    EXPECT_TRUE(contains);
    EXPECT_EQ(actual.number, 0);
}

TEST(RegistryTest, Emplace_SingleValidComponent)
{
    ECS::Registry sut;
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

TEST(RegistryTest, Emplace_MultipleComponent)
{
    ECS::Registry sut;
    ECS::Entity entity{ 0 };

    auto actual1 = sut.Emplace<AComponent>(entity, 42);
    auto actual2 = sut.Emplace<BComponent>(entity, 1, "Test");
    auto contains = sut.IsValid(entity);

    EXPECT_TRUE(contains);
    EXPECT_EQ(actual1.number, 42);
    EXPECT_EQ(actual2.number, 1);
    EXPECT_STREQ(actual2.string, "Test");
}

TEST(RegistryTest, Patch_SingleComponent)
{
    ECS::Registry sut;
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

TEST(RegistryTest, Get_SingleComponent)
{
    ECS::Registry sut;
    ECS::Entity entity{ 0 };

    sut.Emplace<AComponent>(entity, 42);
    sut.Emplace<BComponent>(entity, 1, "Test");

    auto& actual1 = sut.Get<AComponent>(entity);
    auto& actual2 = sut.Get<BComponent>(entity);

    EXPECT_EQ(actual1.number, 42);

    EXPECT_EQ(actual2.number, 1);
    EXPECT_STREQ(actual2.string, "Test");
}

TEST(RegistryTest, Get_MultipleComponent)
{
    ECS::Registry sut;
    ECS::Entity entity{ 0 };

    sut.Emplace<AComponent>(entity, 42);
    sut.Emplace<BComponent>(entity, 1, "Test");

    auto [actual1, actual2] = sut.Get<AComponent, BComponent>(entity);

    EXPECT_EQ(actual1.number, 42);

    EXPECT_EQ(actual2.number, 1);
    EXPECT_STREQ(actual2.string, "Test");
}

TEST(RegistryTest, AnyOf_EmptyEntity_False)
{
    ECS::Registry sut;
    ECS::Entity entity = sut.Create();

    bool actual1 = sut.AnyOf<AComponent>(entity);
    bool actual2 = sut.AnyOf<AComponent, BComponent>(entity);

    EXPECT_FALSE(actual1);
    EXPECT_FALSE(actual2);
}

TEST(RegistryTest, AllOf_EmptyEntity_False)
{
    ECS::Registry sut;
    ECS::Entity entity = sut.Create();

    bool actual1 = sut.AllOf<AComponent>(entity);
    bool actual2 = sut.AllOf<AComponent, BComponent>(entity);

    EXPECT_FALSE(actual1);
    EXPECT_FALSE(actual2);
}

TEST(RegistryTest, AllOf_SingleComponent)
{
    ECS::Registry sut;
    ECS::Entity entity0{ 0 };
    ECS::Entity entity1{ 1 };

    sut.Emplace<AComponent>(entity0, 1);
    sut.Emplace<BComponent>(entity1, 1, "Test");

    bool actual1 = sut.AllOf<AComponent>(entity1);
    bool actual2 = sut.AllOf<BComponent>(entity1);

    EXPECT_FALSE(actual1);
    EXPECT_TRUE(actual2);
}

TEST(RegistryTest, AllOf_MultipleComponent)
{
    ECS::Registry sut;
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

TEST(RegistryTest, AnyOf_SingleComponent)
{
    ECS::Registry sut;
    ECS::Entity entity0{ 0 };
    ECS::Entity entity1{ 1 };

    sut.Emplace<AComponent>(entity0, 1);
    sut.Emplace<BComponent>(entity1, 1, "Test");

    bool actual1 = sut.AnyOf<AComponent>(entity1);
    bool actual2 = sut.AnyOf<BComponent>(entity1);

    EXPECT_FALSE(actual1);
    EXPECT_TRUE(actual2);
}

TEST(RegistryTest, AnyOf_MultipleComponent)
{
    ECS::Registry sut;
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

TEST(RegistryTest, Erase_SingleComponent)
{
    ECS::Registry sut;
    ECS::Entity entity0{ 0 };
    ECS::Entity entity1{ 1 };

    sut.Emplace<AComponent>(entity0, 1);
    sut.Emplace<AComponent>(entity1, 42);
    sut.Emplace<BComponent>(entity1, 1, "Test");

    sut.Erase<AComponent>(entity0);
    sut.Erase<BComponent>(entity1);

    bool actual0 = sut.AllOf<AComponent>(entity0);
    bool actual1 = sut.AllOf<BComponent>(entity0);

    bool actual2 = sut.AllOf<AComponent>(entity1);
    bool actual3 = sut.AllOf<BComponent>(entity1);

    EXPECT_FALSE(actual0);
    EXPECT_FALSE(actual1);

    EXPECT_TRUE(actual2);
    EXPECT_FALSE(actual3);
}

TEST(RegistryTest, Erase_MultipleComponents)
{
    ECS::Registry sut;
    ECS::Entity entity0{ 0 };
    ECS::Entity entity1{ 1 };

    sut.Emplace<AComponent>(entity0, 1);
    sut.Emplace<AComponent>(entity1, 42);
    sut.Emplace<BComponent>(entity1, 1, "Test");

    sut.Erase<AComponent>(entity0);
    sut.Erase<AComponent, BComponent>(entity1);

    bool actual0 = sut.AllOf<AComponent, BComponent>(entity0);
    bool actual1 = sut.AllOf<AComponent, BComponent>(entity0);

    EXPECT_FALSE(actual0);
    EXPECT_FALSE(actual1);
}

TEST(RegistryTest, Query_SingleComponent)
{
    ECS::Registry sut;
    ECS::Entity entity0{ 0 };
    ECS::Entity entity1{ 1 };

    sut.Emplace<AComponent>(entity0, 1);
    sut.Emplace<AComponent>(entity1, 42);
    sut.Emplace<BComponent>(entity1, 1, "Test");

    auto actual1 = sut.Query<AComponent>();
    auto actual2 = sut.Query<BComponent>();

    EXPECT_EQ(actual1[0].number, 1);
    EXPECT_EQ(actual1[1].number, 42);

    EXPECT_EQ(actual2[0].number, 1);
    EXPECT_STREQ(actual2[0].string, "Test");
}

TEST(RegistryTest, Query_MultipleComponent)
{
    ECS::Registry sut;
    ECS::Entity entity0{ 0 };
    ECS::Entity entity1{ 1 };

    sut.Emplace<AComponent>(entity0, 1);
    sut.Emplace<AComponent>(entity1, 42);
    sut.Emplace<BComponent>(entity1, 1, "Test");

    auto [actual1, actual2] = sut.Query<AComponent, BComponent>();

    EXPECT_EQ(actual1[0].number, 1);
    EXPECT_EQ(actual1[1].number, 42);

    EXPECT_EQ(actual2[0].number, 1);
    EXPECT_STREQ(actual2[0].string, "Test");
}

TEST(RegistryTest, IsValid)
{
    ECS::Registry sut;
    ECS::Entity entity = sut.Create();

    bool actual1 = sut.IsValid(entity);
    bool actual2 = sut.IsValid(1);

    EXPECT_TRUE(actual1);
    EXPECT_FALSE(actual2);
}
