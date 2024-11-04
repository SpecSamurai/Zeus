#include <ecs/FamilyId.hpp>

#include <gtest/gtest.h>

TEST(FamilyIdTest, Type_IdPerType)
{
    auto idInt1 = Zeus::FamilyId::Type<int>();
    auto idInt2 = Zeus::FamilyId::Type<int>();

    auto idFloat1 = Zeus::FamilyId::Type<float>();

    EXPECT_EQ(idInt1, 0);
    EXPECT_EQ(idInt2, 0);

    EXPECT_EQ(idFloat1, 1);
}
