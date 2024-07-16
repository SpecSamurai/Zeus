#include <math/Vector2.hpp>

#include <gtest/gtest.h>

TEST(Vector2Test, Constructor_Properties)
{
    auto sut = Zeus::Vector2(0.f, 0.f);

    EXPECT_EQ(0.f, sut.x);
    EXPECT_EQ(0.f, sut.r);
    EXPECT_EQ(0.f, sut.s);

    EXPECT_EQ(0.f, sut.y);
    EXPECT_EQ(0.f, sut.g);
    EXPECT_EQ(0.f, sut.t);

    sut.x = 1.f;
    sut.y = 2.f;

    EXPECT_EQ(1.f, sut.x);
    EXPECT_EQ(1.f, sut.r);
    EXPECT_EQ(1.f, sut.s);

    EXPECT_EQ(2.f, sut.y);
    EXPECT_EQ(2.f, sut.g);
    EXPECT_EQ(2.f, sut.t);
}

TEST(Vector2Test, ArraySubscriptOperator)
{
    auto sut = Zeus::Vector2(0.f, 0.f);

    EXPECT_EQ(0.f, sut[0]);
    EXPECT_EQ(0.f, sut[1]);

    sut[0] = 1.f;
    sut[1] = 2.f;

    EXPECT_EQ(1.f, sut[0]);
    EXPECT_EQ(2.f, sut[1]);
}

TEST(Vector2Test, BinaryAdditionOperator_Vectors)
{
    auto sut1 = Zeus::Vector2(1.f, 2.f);
    auto sut2 = Zeus::Vector2(5.f, 6.f);

    Zeus::Vector2 actual = sut1 + sut2;

    EXPECT_EQ(1.f, sut1.x);
    EXPECT_EQ(2.f, sut1.y);

    EXPECT_EQ(5.f, sut2.x);
    EXPECT_EQ(6.f, sut2.y);

    EXPECT_EQ(6.f, actual.x);
    EXPECT_EQ(8.f, actual.y);
}

TEST(Vector2Test, BinaryAdditionOperator_VectorWithScalar)
{
    auto sut = Zeus::Vector2(1.f, 2.f);
    float scalar = 1.f;

    Zeus::Vector2 actual1 = scalar + sut;
    Zeus::Vector2 actual2 = sut + scalar;

    EXPECT_EQ(1.f, sut.x);
    EXPECT_EQ(2.f, sut.y);
    EXPECT_EQ(1.f, scalar);

    EXPECT_EQ(2.f, actual1.x);
    EXPECT_EQ(3.f, actual1.y);

    EXPECT_EQ(2.f, actual2.x);
    EXPECT_EQ(3.f, actual2.y);
}

TEST(Vector2Test, BinarySubtractionOperator_Vectors)
{
    auto sut1 = Zeus::Vector2(10.f, 9.f);
    auto sut2 = Zeus::Vector2(1.f, 2.f);

    Zeus::Vector2 actual1 = sut1 - sut2;
    Zeus::Vector2 actual2 = sut2 - sut1;

    EXPECT_EQ(10.f, sut1.x);
    EXPECT_EQ(9.f, sut1.y);

    EXPECT_EQ(1.f, sut2.x);
    EXPECT_EQ(2.f, sut2.y);

    EXPECT_EQ(9.f, actual1.x);
    EXPECT_EQ(7.f, actual1.y);

    EXPECT_EQ(-9.f, actual2.x);
    EXPECT_EQ(-7.f, actual2.y);
}

TEST(Vector2Test, BinarySubtractionOperator_VectorWithScalar)
{
    auto sut = Zeus::Vector2(10.f, 9.f);
    float scalar = 1.f;

    Zeus::Vector2 actual1 = sut - scalar;
    Zeus::Vector2 actual2 = scalar - sut;

    EXPECT_EQ(10.f, sut.x);
    EXPECT_EQ(9.f, sut.y);
    EXPECT_EQ(1.f, scalar);

    EXPECT_EQ(9.f, actual1.x);
    EXPECT_EQ(8.f, actual1.y);

    EXPECT_EQ(-9.f, actual2.x);
    EXPECT_EQ(-8.f, actual2.y);
}

TEST(Vector2Test, BinaryMultiplicationOperator_VectorWithScalar)
{
    auto sut = Zeus::Vector2(1.f, 2.f);
    float scalar = 2.f;

    Zeus::Vector2 actual1 = sut * scalar;
    Zeus::Vector2 actual2 = scalar * sut;

    EXPECT_EQ(1.f, sut.x);
    EXPECT_EQ(2.f, sut.y);
    EXPECT_EQ(2.f, scalar);

    EXPECT_EQ(2.f, actual1.x);
    EXPECT_EQ(4.f, actual1.y);

    EXPECT_EQ(2.f, actual2.x);
    EXPECT_EQ(4.f, actual2.y);
}

TEST(Vector2Test, BinaryDivisionOperator_VectorWithScalar)
{
    auto sut = Zeus::Vector2(1.f, 2.f);
    float scalar = 2.f;

    Zeus::Vector2 actual1 = sut / scalar;
    Zeus::Vector2 actual2 = scalar / sut;

    EXPECT_EQ(1.f, sut.x);
    EXPECT_EQ(2.f, sut.y);
    EXPECT_EQ(2.f, scalar);

    EXPECT_EQ(0.5f, actual1.x);
    EXPECT_EQ(1.f, actual1.y);

    EXPECT_EQ(2.f, actual2.x);
    EXPECT_EQ(1.f, actual2.y);
}
