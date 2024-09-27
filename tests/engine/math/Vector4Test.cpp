#include <math/Vector4.hpp>

#include <gtest/gtest.h>

TEST(Vector4Test, Constructor_Properties)
{
    auto sut = Zeus::Vector4(0.f, 0.f, 0.f, 0.f);

    EXPECT_EQ(0.f, sut.x);
    EXPECT_EQ(0.f, sut.r);
    EXPECT_EQ(0.f, sut.s);

    EXPECT_EQ(0.f, sut.y);
    EXPECT_EQ(0.f, sut.g);
    EXPECT_EQ(0.f, sut.t);

    EXPECT_EQ(0.f, sut.z);
    EXPECT_EQ(0.f, sut.b);
    EXPECT_EQ(0.f, sut.p);

    EXPECT_EQ(0.f, sut.w);
    EXPECT_EQ(0.f, sut.a);
    EXPECT_EQ(0.f, sut.q);

    sut.x = 1.f;
    sut.y = 2.f;
    sut.z = 3.f;
    sut.w = 4.f;

    EXPECT_EQ(1.f, sut.x);
    EXPECT_EQ(1.f, sut.r);
    EXPECT_EQ(1.f, sut.s);

    EXPECT_EQ(2.f, sut.y);
    EXPECT_EQ(2.f, sut.g);
    EXPECT_EQ(2.f, sut.t);

    EXPECT_EQ(3.f, sut.z);
    EXPECT_EQ(3.f, sut.b);
    EXPECT_EQ(3.f, sut.p);

    EXPECT_EQ(4.f, sut.w);
    EXPECT_EQ(4.f, sut.a);
    EXPECT_EQ(4.f, sut.q);
}

TEST(Vector4Test, ArraySubscriptOperator)
{
    auto sut = Zeus::Vector4(0.f, 0.f, 0.f, 0.f);

    EXPECT_EQ(0.f, sut[0]);
    EXPECT_EQ(0.f, sut[1]);
    EXPECT_EQ(0.f, sut[2]);
    EXPECT_EQ(0.f, sut[3]);

    sut[0] = 1.f;
    sut[1] = 2.f;
    sut[2] = 3.f;
    sut[3] = 4.f;

    EXPECT_EQ(1.f, sut[0]);
    EXPECT_EQ(2.f, sut[1]);
    EXPECT_EQ(3.f, sut[2]);
    EXPECT_EQ(4.f, sut[3]);
}

TEST(Vector4Test, BinaryAdditionOperator_Vectors)
{
    auto sut1 = Zeus::Vector4(1.f, 2.f, 3.f, 4.f);
    auto sut2 = Zeus::Vector4(5.f, 6.f, 7.f, 8.f);

    Zeus::Vector4 actual = sut1 + sut2;

    EXPECT_EQ(1.f, sut1.x);
    EXPECT_EQ(2.f, sut1.y);
    EXPECT_EQ(3.f, sut1.z);
    EXPECT_EQ(4.f, sut1.w);

    EXPECT_EQ(5.f, sut2.x);
    EXPECT_EQ(6.f, sut2.y);
    EXPECT_EQ(7.f, sut2.z);
    EXPECT_EQ(8.f, sut2.w);

    EXPECT_EQ(6.f, actual.x);
    EXPECT_EQ(8.f, actual.y);
    EXPECT_EQ(10.f, actual.z);
    EXPECT_EQ(12.f, actual.w);
}

TEST(Vector4Test, BinaryAdditionOperator_VectorWithScalar)
{
    auto sut = Zeus::Vector4(1.f, 2.f, 3.f, 4.f);
    float scalar = 1.f;

    Zeus::Vector4 actual1 = scalar + sut;
    Zeus::Vector4 actual2 = sut + scalar;

    EXPECT_EQ(1.f, sut.x);
    EXPECT_EQ(2.f, sut.y);
    EXPECT_EQ(3.f, sut.z);
    EXPECT_EQ(4.f, sut.w);
    EXPECT_EQ(1.f, scalar);

    EXPECT_EQ(2.f, actual1.x);
    EXPECT_EQ(3.f, actual1.y);
    EXPECT_EQ(4.f, actual1.z);
    EXPECT_EQ(5.f, actual1.w);

    EXPECT_EQ(2.f, actual2.x);
    EXPECT_EQ(3.f, actual2.y);
    EXPECT_EQ(4.f, actual2.z);
    EXPECT_EQ(5.f, actual2.w);
}

TEST(Vector4Test, UnarySubtractionOperator_Vector)
{
    auto sut = Zeus::Vector4(1.f, 2.f, 3.f, 4.f);

    auto actual = -sut;

    EXPECT_EQ(-1.f, actual.x);
    EXPECT_EQ(-2.f, actual.y);
    EXPECT_EQ(-3.f, actual.z);
    EXPECT_EQ(-4.f, actual.w);
}

TEST(Vector4Test, BinarySubtractionOperator_Vectors)
{
    auto sut1 = Zeus::Vector4(10.f, 9.f, 8.f, 7.f);
    auto sut2 = Zeus::Vector4(1.f, 2.f, 3.f, 4.f);

    Zeus::Vector4 actual1 = sut1 - sut2;
    Zeus::Vector4 actual2 = sut2 - sut1;

    EXPECT_EQ(10.f, sut1.x);
    EXPECT_EQ(9.f, sut1.y);
    EXPECT_EQ(8.f, sut1.z);
    EXPECT_EQ(7.f, sut1.w);

    EXPECT_EQ(1.f, sut2.x);
    EXPECT_EQ(2.f, sut2.y);
    EXPECT_EQ(3.f, sut2.z);
    EXPECT_EQ(4.f, sut2.w);

    EXPECT_EQ(9.f, actual1.x);
    EXPECT_EQ(7.f, actual1.y);
    EXPECT_EQ(5.f, actual1.z);
    EXPECT_EQ(3.f, actual1.w);

    EXPECT_EQ(-9.f, actual2.x);
    EXPECT_EQ(-7.f, actual2.y);
    EXPECT_EQ(-5.f, actual2.z);
    EXPECT_EQ(-3.f, actual2.w);
}

TEST(Vector4Test, BinarySubtractionOperator_VectorWithScalar)
{
    auto sut = Zeus::Vector4(10.f, 9.f, 8.f, 7.f);
    float scalar = 1.f;

    Zeus::Vector4 actual1 = sut - scalar;
    Zeus::Vector4 actual2 = scalar - sut;

    EXPECT_EQ(10.f, sut.x);
    EXPECT_EQ(9.f, sut.y);
    EXPECT_EQ(8.f, sut.z);
    EXPECT_EQ(7.f, sut.w);
    EXPECT_EQ(1.f, scalar);

    EXPECT_EQ(9.f, actual1.x);
    EXPECT_EQ(8.f, actual1.y);
    EXPECT_EQ(7.f, actual1.z);
    EXPECT_EQ(6.f, actual1.w);

    EXPECT_EQ(-9.f, actual2.x);
    EXPECT_EQ(-8.f, actual2.y);
    EXPECT_EQ(-7.f, actual2.z);
    EXPECT_EQ(-6.f, actual2.w);
}

TEST(Vector4Test, BinaryMultiplicationOperator_VectorWithScalar)
{
    auto sut = Zeus::Vector4(1.f, 2.f, 3.f, 4.f);
    float scalar = 2.f;

    Zeus::Vector4 actual1 = sut * scalar;
    Zeus::Vector4 actual2 = scalar * sut;

    EXPECT_EQ(1.f, sut.x);
    EXPECT_EQ(2.f, sut.y);
    EXPECT_EQ(3.f, sut.z);
    EXPECT_EQ(4.f, sut.w);
    EXPECT_EQ(2.f, scalar);

    EXPECT_EQ(2.f, actual1.x);
    EXPECT_EQ(4.f, actual1.y);
    EXPECT_EQ(6.f, actual1.z);
    EXPECT_EQ(8.f, actual1.w);

    EXPECT_EQ(2.f, actual2.x);
    EXPECT_EQ(4.f, actual2.y);
    EXPECT_EQ(6.f, actual2.z);
    EXPECT_EQ(8.f, actual2.w);
}

TEST(Vector4Test, BinaryDivisionOperator_VectorWithScalar)
{
    auto sut = Zeus::Vector4(1.f, 2.f, 3.f, 4.f);
    float scalar = 2.f;

    Zeus::Vector4 actual1 = sut / scalar;
    Zeus::Vector4 actual2 = scalar / sut;

    EXPECT_EQ(1.f, sut.x);
    EXPECT_EQ(2.f, sut.y);
    EXPECT_EQ(3.f, sut.z);
    EXPECT_EQ(4.f, sut.w);
    EXPECT_EQ(2.f, scalar);

    EXPECT_EQ(0.5f, actual1.x);
    EXPECT_EQ(1.f, actual1.y);
    EXPECT_EQ(1.5f, actual1.z);
    EXPECT_EQ(2.f, actual1.w);

    EXPECT_EQ(2.f, actual2.x);
    EXPECT_EQ(1.f, actual2.y);
    EXPECT_EQ(2.f / 3.f, actual2.z);
    EXPECT_EQ(0.5f, actual2.w);
}
