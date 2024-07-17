#include "math/trigonometric.hpp"
#include "math/definitions.hpp"

#include <gtest/gtest.h>

TEST(TrigonometricTest, Radians)
{
    EXPECT_EQ(0.f, Zeus::radians(0.f));
    EXPECT_EQ(0.5235987756f, Zeus::radians(30.f));
    EXPECT_EQ(0.7853981634f, Zeus::radians(45.f));
    EXPECT_EQ(1.0471975512f, Zeus::radians(60.f));
    EXPECT_EQ(1.5707963268f, Zeus::radians(90.f));
    EXPECT_EQ(2.0943951024f, Zeus::radians(120.f));
    EXPECT_EQ(2.3561944902f, Zeus::radians(135.f));
    EXPECT_EQ(2.6179938780f, Zeus::radians(150.f));
    EXPECT_EQ(3.1415926536f, Zeus::radians(180.f));
    EXPECT_EQ(4.7123889804f, Zeus::radians(270.f));
    EXPECT_EQ(6.2831853072f, Zeus::radians(360.f));
}

TEST(TrigonometricTest, Degrees)
{
    EXPECT_EQ(0, Zeus::degrees(0.f));
    EXPECT_EQ(30, Zeus::degrees(Zeus::PI / 6.f));
    EXPECT_EQ(45, Zeus::degrees(Zeus::PI / 4.f));
    EXPECT_EQ(60, Zeus::degrees(Zeus::PI / 3.f));
    EXPECT_EQ(90, Zeus::degrees(Zeus::PI / 2.f));
    EXPECT_EQ(120, Zeus::degrees(2 * Zeus::PI / 3.f));
    EXPECT_EQ(135, Zeus::degrees(3 * Zeus::PI / 4.f));
    EXPECT_EQ(150, Zeus::degrees(5 * Zeus::PI / 6.f));
    EXPECT_EQ(180, Zeus::degrees(Zeus::PI));
    EXPECT_EQ(270, Zeus::degrees(3 * Zeus::PI / 2.f));
    EXPECT_EQ(360, Zeus::degrees(2 * Zeus::PI));
}
