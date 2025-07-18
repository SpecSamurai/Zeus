#include <math/Vector3.hpp>

#include <gtest/gtest.h>

TEST(Vector3Test, Constructor_Properties)
{
    auto sut = Zeus::Math::Vector3(0.f, 0.f, 0.f);

    EXPECT_EQ(0.f, sut.x);
    EXPECT_EQ(0.f, sut.r);
    EXPECT_EQ(0.f, sut.s);

    EXPECT_EQ(0.f, sut.y);
    EXPECT_EQ(0.f, sut.g);
    EXPECT_EQ(0.f, sut.t);

    EXPECT_EQ(0.f, sut.z);
    EXPECT_EQ(0.f, sut.b);
    EXPECT_EQ(0.f, sut.p);

    sut.x = 1.f;
    sut.y = 2.f;
    sut.z = 3.f;

    EXPECT_EQ(1.f, sut.x);
    EXPECT_EQ(1.f, sut.r);
    EXPECT_EQ(1.f, sut.s);

    EXPECT_EQ(2.f, sut.y);
    EXPECT_EQ(2.f, sut.g);
    EXPECT_EQ(2.f, sut.t);

    EXPECT_EQ(3.f, sut.z);
    EXPECT_EQ(3.f, sut.b);
    EXPECT_EQ(3.f, sut.p);
}

TEST(Vector3Test, ArraySubscriptOperator)
{
    auto sut = Zeus::Math::Vector3(0.f, 0.f, 0.f);

    EXPECT_EQ(0.f, sut[0]);
    EXPECT_EQ(0.f, sut[1]);
    EXPECT_EQ(0.f, sut[2]);

    sut[0] = 1.f;
    sut[1] = 2.f;
    sut[2] = 3.f;

    EXPECT_EQ(1.f, sut[0]);
    EXPECT_EQ(2.f, sut[1]);
    EXPECT_EQ(3.f, sut[2]);
}

TEST(Vector3Test, BinaryAdditionOperator_Vectors)
{
    auto sut1 = Zeus::Math::Vector3(1.f, 2.f, 3.f);
    auto sut2 = Zeus::Math::Vector3(5.f, 6.f, 7.f);

    Zeus::Math::Vector3 actual = sut1 + sut2;

    EXPECT_EQ(1.f, sut1.x);
    EXPECT_EQ(2.f, sut1.y);
    EXPECT_EQ(3.f, sut1.z);

    EXPECT_EQ(5.f, sut2.x);
    EXPECT_EQ(6.f, sut2.y);
    EXPECT_EQ(7.f, sut2.z);

    EXPECT_EQ(6.f, actual.x);
    EXPECT_EQ(8.f, actual.y);
    EXPECT_EQ(10.f, actual.z);
}

TEST(Vector3Test, BinaryAdditionOperator_VectorWithScalar)
{
    auto sut = Zeus::Math::Vector3(1.f, 2.f, 3.f);
    float scalar = 1.f;

    Zeus::Math::Vector3 actual1 = scalar + sut;
    Zeus::Math::Vector3 actual2 = sut + scalar;

    EXPECT_EQ(1.f, sut.x);
    EXPECT_EQ(2.f, sut.y);
    EXPECT_EQ(3.f, sut.z);
    EXPECT_EQ(1.f, scalar);

    EXPECT_EQ(2.f, actual1.x);
    EXPECT_EQ(3.f, actual1.y);
    EXPECT_EQ(4.f, actual1.z);

    EXPECT_EQ(2.f, actual2.x);
    EXPECT_EQ(3.f, actual2.y);
    EXPECT_EQ(4.f, actual2.z);
}

TEST(Vector3Test, UnarySubtractionOperator_Vector)
{
    auto sut = Zeus::Math::Vector3(1.f, 2.f, 3.f);

    auto actual = -sut;

    EXPECT_EQ(-1.f, actual.x);
    EXPECT_EQ(-2.f, actual.y);
    EXPECT_EQ(-3.f, actual.z);
}

TEST(Vector3Test, BinarySubtractionOperator_Vectors)
{
    auto sut1 = Zeus::Math::Vector3(10.f, 9.f, 8.f);
    auto sut2 = Zeus::Math::Vector3(1.f, 2.f, 3.f);

    Zeus::Math::Vector3 actual1 = sut1 - sut2;
    Zeus::Math::Vector3 actual2 = sut2 - sut1;

    EXPECT_EQ(10.f, sut1.x);
    EXPECT_EQ(9.f, sut1.y);
    EXPECT_EQ(8.f, sut1.z);

    EXPECT_EQ(1.f, sut2.x);
    EXPECT_EQ(2.f, sut2.y);
    EXPECT_EQ(3.f, sut2.z);

    EXPECT_EQ(9.f, actual1.x);
    EXPECT_EQ(7.f, actual1.y);
    EXPECT_EQ(5.f, actual1.z);

    EXPECT_EQ(-9.f, actual2.x);
    EXPECT_EQ(-7.f, actual2.y);
    EXPECT_EQ(-5.f, actual2.z);
}

TEST(Vector3Test, BinarySubtractionOperator_VectorWithScalar)
{
    auto sut = Zeus::Math::Vector3(10.f, 9.f, 8.f);
    float scalar = 1.f;

    Zeus::Math::Vector3 actual1 = sut - scalar;
    Zeus::Math::Vector3 actual2 = scalar - sut;

    EXPECT_EQ(10.f, sut.x);
    EXPECT_EQ(9.f, sut.y);
    EXPECT_EQ(8.f, sut.z);
    EXPECT_EQ(1.f, scalar);

    EXPECT_EQ(9.f, actual1.x);
    EXPECT_EQ(8.f, actual1.y);
    EXPECT_EQ(7.f, actual1.z);

    EXPECT_EQ(-9.f, actual2.x);
    EXPECT_EQ(-8.f, actual2.y);
    EXPECT_EQ(-7.f, actual2.z);
}

TEST(Vector3Test, BinaryMultiplicationOperator_VectorWithScalar)
{
    auto sut = Zeus::Math::Vector3(1.f, 2.f, 3.f);
    float scalar = 2.f;

    Zeus::Math::Vector3 actual1 = sut * scalar;
    Zeus::Math::Vector3 actual2 = scalar * sut;

    EXPECT_EQ(1.f, sut.x);
    EXPECT_EQ(2.f, sut.y);
    EXPECT_EQ(3.f, sut.z);
    EXPECT_EQ(2.f, scalar);

    EXPECT_EQ(2.f, actual1.x);
    EXPECT_EQ(4.f, actual1.y);
    EXPECT_EQ(6.f, actual1.z);

    EXPECT_EQ(2.f, actual2.x);
    EXPECT_EQ(4.f, actual2.y);
    EXPECT_EQ(6.f, actual2.z);
}

TEST(Vector3Test, BinaryMultiplicationOperator_Vectors)
{
    auto sut1 = Zeus::Math::Vector3(1.f, 2.f, 3.f);
    auto sut2 = Zeus::Math::Vector3(10.f, 20.f, 30.f);

    Zeus::Math::Vector3 actual1 = sut1 * sut2;
    Zeus::Math::Vector3 actual2 = sut2 * sut1;

    EXPECT_EQ(10.f, actual1.x);
    EXPECT_EQ(40.f, actual1.y);
    EXPECT_EQ(90.f, actual1.z);

    EXPECT_EQ(10.f, actual2.x);
    EXPECT_EQ(40.f, actual2.y);
    EXPECT_EQ(90.f, actual2.z);
}

TEST(Vector3Test, BinaryDivisionOperator_VectorWithScalar)
{
    auto sut = Zeus::Math::Vector3(1.f, 2.f, 3.f);
    float scalar = 2.f;

    Zeus::Math::Vector3 actual1 = sut / scalar;
    Zeus::Math::Vector3 actual2 = scalar / sut;

    EXPECT_EQ(1.f, sut.x);
    EXPECT_EQ(2.f, sut.y);
    EXPECT_EQ(3.f, sut.z);
    EXPECT_EQ(2.f, scalar);

    EXPECT_EQ(0.5f, actual1.x);
    EXPECT_EQ(1.f, actual1.y);
    EXPECT_EQ(1.5f, actual1.z);

    EXPECT_EQ(2.f, actual2.x);
    EXPECT_EQ(1.f, actual2.y);
    EXPECT_EQ(2.f / 3.f, actual2.z);
}
