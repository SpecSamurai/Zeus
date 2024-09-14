#include <math/Quaternion.hpp>

#include <gtest/gtest.h>

TEST(QuaternionTest, Identity_Quaternion)
{
    auto sut = Zeus::Quaternion<float>();

    EXPECT_EQ(0.f, sut.x);
    EXPECT_EQ(0.f, sut.y);
    EXPECT_EQ(0.f, sut.z);
    EXPECT_EQ(1.f, sut.w);

    EXPECT_EQ(0.f, sut.complex().x);
    EXPECT_EQ(0.f, sut.complex().y);
    EXPECT_EQ(0.f, sut.complex().z);
    EXPECT_EQ(1.f, sut.real());
}

TEST(QuaternionTest, ArraySubscriptOperator)
{
    auto sut = Zeus::Quaternion(0.f, 0.f, 0.f, 0.f);

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
    EXPECT_EQ(1.f, sut.x);
    EXPECT_EQ(2.f, sut.y);
    EXPECT_EQ(3.f, sut.z);
    EXPECT_EQ(4.f, sut.w);
}

TEST(QuaternionTest, UnarySubtractionOperator_Quaternions)
{
    auto sut = Zeus::Quaternion(1.f, 2.f, 3.f, 4.f);

    Zeus::Quaternion actual = -sut;

    EXPECT_EQ(-1.f, actual.x);
    EXPECT_EQ(-2.f, actual.y);
    EXPECT_EQ(-3.f, actual.z);
    EXPECT_EQ(-4.f, actual.w);
}

TEST(QuaternionTest, BinaryAdditionOperator_Quaternions)
{
    auto sut1 = Zeus::Quaternion(1.f, 2.f, 3.f, 4.f);
    auto sut2 = Zeus::Quaternion(5.f, 6.f, 7.f, 8.f);

    Zeus::Quaternion actual = sut1 + sut2;

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

TEST(QuaternionTest, BinaryAdditionOperator_QuaternionWithScalar)
{
    auto sut = Zeus::Quaternion(1.f, 2.f, 3.f, 4.f);
    float scalar = 1.f;

    Zeus::Quaternion actual1 = scalar + sut;
    Zeus::Quaternion actual2 = sut + scalar;

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

TEST(QuaternionTest, BinarySubtractionOperator_Quaternions)
{
    auto sut1 = Zeus::Quaternion(10.f, 9.f, 8.f, 7.f);
    auto sut2 = Zeus::Quaternion(1.f, 2.f, 3.f, 4.f);

    Zeus::Quaternion actual1 = sut1 - sut2;
    Zeus::Quaternion actual2 = sut2 - sut1;

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

TEST(QuaternionTest, BinarySubtractionOperator_QuaternionWithScalar)
{
    auto sut = Zeus::Quaternion(10.f, 9.f, 8.f, 7.f);
    float scalar = 1.f;

    Zeus::Quaternion actual1 = sut - scalar;
    Zeus::Quaternion actual2 = scalar - sut;

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

TEST(QuaternionTest, BinaryMultiplicationOperator_QuaternionWithScalar)
{
    auto sut = Zeus::Quaternion(1.f, 2.f, 3.f, 4.f);
    float scalar = 2.f;

    Zeus::Quaternion actual1 = sut * scalar;
    Zeus::Quaternion actual2 = scalar * sut;

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

TEST(QuaternionTest, BinaryDivisionOperator_QuaternionWithScalar)
{
    auto sut = Zeus::Quaternion(1.f, 2.f, 3.f, 4.f);
    float scalar = 2.f;

    Zeus::Quaternion actual1 = sut / scalar;
    Zeus::Quaternion actual2 = scalar / sut;

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
