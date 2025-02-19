#include <math/definitions.hpp>
#include <math/trigonometric.hpp>

#include <gtest/gtest.h>

TEST(TrigonometricTest, Radians)
{
    EXPECT_EQ(0.f, Zeus::Math::radians(0.f));
    EXPECT_EQ(0.5235987756f, Zeus::Math::radians(30.f));
    EXPECT_EQ(0.7853981634f, Zeus::Math::radians(45.f));
    EXPECT_EQ(1.0471975512f, Zeus::Math::radians(60.f));
    EXPECT_EQ(1.5707963268f, Zeus::Math::radians(90.f));
    EXPECT_EQ(2.0943951024f, Zeus::Math::radians(120.f));
    EXPECT_EQ(2.3561944902f, Zeus::Math::radians(135.f));
    EXPECT_EQ(2.6179938780f, Zeus::Math::radians(150.f));
    EXPECT_EQ(3.1415926536f, Zeus::Math::radians(180.f));
    EXPECT_EQ(4.7123889804f, Zeus::Math::radians(270.f));
    EXPECT_EQ(6.2831853072f, Zeus::Math::radians(360.f));
}

TEST(TrigonometricTest, Degrees)
{
    EXPECT_EQ(0, Zeus::Math::degrees(0.f));
    EXPECT_EQ(30, Zeus::Math::degrees(Zeus::Math::PI / 6.f));
    EXPECT_EQ(45, Zeus::Math::degrees(Zeus::Math::PI / 4.f));
    EXPECT_EQ(60, Zeus::Math::degrees(Zeus::Math::PI / 3.f));
    EXPECT_EQ(90, Zeus::Math::degrees(Zeus::Math::PI / 2.f));
    EXPECT_EQ(120, Zeus::Math::degrees(2 * Zeus::Math::PI / 3.f));
    EXPECT_EQ(135, Zeus::Math::degrees(3 * Zeus::Math::PI / 4.f));
    EXPECT_EQ(150, Zeus::Math::degrees(5 * Zeus::Math::PI / 6.f));
    EXPECT_EQ(180, Zeus::Math::degrees(Zeus::Math::PI));
    EXPECT_EQ(270, Zeus::Math::degrees(3 * Zeus::Math::PI / 2.f));
    EXPECT_EQ(360, Zeus::Math::degrees(2 * Zeus::Math::PI));
}
