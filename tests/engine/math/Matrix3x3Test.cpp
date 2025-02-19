#include <math/Matrix3x3.hpp>
#include <math/Vector3.hpp>

#include <gtest/gtest.h>

TEST(Matrix3x3Test, Constructor_ArraySubscriptOperator)
{
    auto sut = Zeus::Math::Matrix3x3(
        // clang-format off
        0.f, 0.f, 0.f,
        0.f, 0.f, 0.f,
        0.f, 0.f, 0.f
        // clang-format on
    );

    for (std::size_t i{ 0 }; i < 3; ++i)
        for (std::size_t j{ 0 }; j < 3; ++j)
            EXPECT_EQ(0.f, sut[i][j]);

    float temp{ 1.f };
    for (std::size_t i{ 0 }; i < 3; ++i)
        for (std::size_t j{ 0 }; j < 3; ++j)
            sut[i][j] = temp++;

    temp = 1.f;
    for (std::size_t i{ 0 }; i < 3; ++i)
        for (std::size_t j{ 0 }; j < 3; ++j)
            EXPECT_EQ(temp++, sut[i][j]);
}

TEST(Matrix3x3Test, Constructor_Scalar)
{
    auto sut = Zeus::Math::Matrix3x3(1.f);

    for (std::size_t i{ 0 }; i < 3; ++i)
        for (std::size_t j{ 0 }; j < 3; ++j)
            if (i == j)
                EXPECT_EQ(1.f, sut[i][j]);
            else
                EXPECT_EQ(0, sut[i][j]);
}

TEST(Matrix3x3Test, BinaryAdditionOperator_Matrices)
{
    auto sut1 = Zeus::Math::Matrix3x3(
        // clang-format off
        1.f, 2.f, 3.f,
        4.f, 5.f, 6.f,
        7.f, 8.f, 9.f
        // clang-format on
    );

    auto sut2 = Zeus::Math::Matrix3x3(
        // clang-format off
        10.f, 11.f, 12.f,
        13.f, 14.f, 15.f,
        16.f, 17.f, 18.f
        // clang-format on
    );

    auto expected = Zeus::Math::Matrix3x3(
        // clang-format off
        11.f, 13.f, 15.f,
        17.f, 19.f, 21.f,
        23.f, 25.f, 27.f
        // clang-format on
    );

    auto actual = sut1 + sut2;

    for (std::size_t i{ 0 }; i < 3; ++i)
        for (std::size_t j{ 0 }; j < 3; ++j)
            EXPECT_EQ(expected[i][j], actual[i][j]);
}

TEST(Matrix3x3Test, BinaryAdditionOperator_MatrixWithScalar)
{
    auto sut = Zeus::Math::Matrix3x3(
        // clang-format off
        1.f, 2.f, 3.f,
        4.f, 5.f, 6.f,
        7.f, 8.f, 9.f
        // clang-format on
    );

    float scalar = 1.f;

    auto expected = Zeus::Math::Matrix3x3(
        // clang-format off
        2.f, 3.f, 4.f,
        5.f, 6.f, 7.f,
        8.f, 9.f, 10.f
        // clang-format on
    );

    auto actual1 = scalar + sut;
    auto actual2 = sut + scalar;

    for (std::size_t i{ 0 }; i < 3; ++i)
        for (std::size_t j{ 0 }; j < 3; ++j)
            EXPECT_EQ(expected[i][j], actual1[i][j]);

    for (std::size_t i{ 0 }; i < 3; ++i)
        for (std::size_t j{ 0 }; j < 3; ++j)
            EXPECT_EQ(expected[i][j], actual2[i][j]);
}

TEST(Matrix3x3Test, BinarySubtractionOperator_Matrices)
{
    auto sut1 = Zeus::Math::Matrix3x3(
        // clang-format off
        9.f, 8.f, 7.f,
        6.f, 5.f, 4.f,
        3.f, 2.f, 1.f
        // clang-format on
    );

    auto sut2 = Zeus::Math::Matrix3x3(
        // clang-format off
        1.f, 2.f, 3.f,
        4.f, 5.f, 6.f,
        7.f, 8.f, 9.f
        // clang-format on
    );

    auto expected1 = Zeus::Math::Matrix3x3(
        // clang-format off
        8.f, 6.f, 4.f,
        2.f, 0.f, -2.f,
        -4.f, -6.f, -8.f
        // clang-format on
    );

    auto expected2 = Zeus::Math::Matrix3x3(
        // clang-format off
        -8.f, -6.f, -4.f,
        -2.f, 0.f, 2.f,
        4.f, 6.f, 8.f
        // clang-format on
    );

    auto actual1 = sut1 - sut2;
    auto actual2 = sut2 - sut1;

    for (std::size_t i{ 0 }; i < 3; ++i)
        for (std::size_t j{ 0 }; j < 3; ++j)
            EXPECT_EQ(expected1[i][j], actual1[i][j]);

    for (std::size_t i{ 0 }; i < 3; ++i)
        for (std::size_t j{ 0 }; j < 3; ++j)
            EXPECT_EQ(expected2[i][j], actual2[i][j]);
}

TEST(Matrix3x3Test, BinarySubtractionOperator_MatrixWithScalar)
{
    auto sut = Zeus::Math::Matrix3x3(
        // clang-format off
        1.f, 2.f, 3.f,
        5.f, 6.f, 7.f,
        9.f, 10.f, 11.f
        // clang-format on
    );

    float scalar = 1.f;

    auto expected1 = Zeus::Math::Matrix3x3(
        // clang-format off
        0.f, -1.f, -2.f,
        -4.f, -5.f, -6.f,
        -8.f, -9.f, -10.f
        // clang-format on
    );

    auto expected2 = Zeus::Math::Matrix3x3(
        // clang-format off
        0.f, 1.f, 2.f,
        4.f, 5.f, 6.f,
        8.f, 9.f, 10.f
        // clang-format on
    );

    auto actual1 = scalar - sut;
    auto actual2 = sut - scalar;

    for (std::size_t i{ 0 }; i < 3; ++i)
        for (std::size_t j{ 0 }; j < 3; ++j)
            EXPECT_EQ(expected1[i][j], actual1[i][j]);

    for (std::size_t i{ 0 }; i < 3; ++i)
        for (std::size_t j{ 0 }; j < 3; ++j)
            EXPECT_EQ(expected2[i][j], actual2[i][j]);
}

TEST(Matrix3x3Test, BinaryMultiplicationOperator_MatrixWithScalar)
{
    auto sut = Zeus::Math::Matrix3x3(
        // clang-format off
        1.f, 2.f, 3.f,
        5.f, 6.f, 7.f,
        9.f, 10.f, 11.f
        // clang-format on
    );

    float scalar = 2.f;

    auto expected = Zeus::Math::Matrix3x3(
        // clang-format off
        2.f, 4.f, 6.f,
        10.f, 12.f, 14.f,
        18.f, 20.f, 22.f
        // clang-format on
    );

    auto actual1 = scalar * sut;
    auto actual2 = sut * scalar;

    for (std::size_t i{ 0 }; i < 3; ++i)
        for (std::size_t j{ 0 }; j < 3; ++j)
            EXPECT_EQ(expected[i][j], actual1[i][j]);

    for (std::size_t i{ 0 }; i < 3; ++i)
        for (std::size_t j{ 0 }; j < 3; ++j)
            EXPECT_EQ(expected[i][j], actual2[i][j]);
}

TEST(Matrix3x3Test, BinaryDivisionOperator_MatrixWithScalar)
{
    auto sut = Zeus::Math::Matrix3x3(
        // clang-format off
        2.f, 4.f, 6.f,
        10.f, 12.f, 14.f,
        18.f, 20.f, 22.f
        // clang-format on
    );

    float scalar = 2.f;

    auto expected1 = Zeus::Math::Matrix3x3(
        // clang-format off
        2.f / 2.f, 2.f / 4.f, 2.f / 6.f,
        2.f / 10.f, 2.f / 12.f, 2.f / 14.f,
        2.f / 18.f, 2.f / 20.f, 2.f / 22.f
        // clang-format on
    );

    auto expected2 = Zeus::Math::Matrix3x3(
        // clang-format off
        1.f, 2.f, 3.f,
        5.f, 6.f, 7.f,
        9.f, 10.f, 11.f
        // clang-format on
    );

    auto actual1 = scalar / sut;
    auto actual2 = sut / scalar;

    for (std::size_t i{ 0 }; i < 3; ++i)
        for (std::size_t j{ 0 }; j < 3; ++j)
            EXPECT_EQ(expected1[i][j], actual1[i][j]);

    for (std::size_t i{ 0 }; i < 3; ++i)
        for (std::size_t j{ 0 }; j < 3; ++j)
            EXPECT_EQ(expected2[i][j], actual2[i][j]);
}

TEST(Matrix3x3Test, BinaryMultiplicationOperator_MatrixWithMatrix)
{
    auto sut1 = Zeus::Math::Matrix3x3(
        // clang-format off
        1.f, 4.f, 7.f,
        2.f, 5.f, 8.f,
        3.f, 6.f, 9.f
        // clang-format on
    );

    auto sut2 = Zeus::Math::Matrix3x3(
        // clang-format off
        10.f, 13.f, 16.f,
        11.f, 14.f, 17.f,
        12.f, 15.f, 18.f
        // clang-format on
    );

    Zeus::Math::Matrix3x3 expected1(
        // clang-format off
        84.f, 201.f, 318.f,
        90.f, 216.f, 342.f,
        96.f, 231.f, 366.f
        // clang-format on
    );

    Zeus::Math::Matrix3x3 expected2(
        // clang-format off
        138.f, 174.f, 210.f,
        171.f, 216.f, 261.f,
        204.f, 258.f, 312.f
        // clang-format on
    );

    auto actual1 = sut1 * sut2;
    auto actual2 = sut2 * sut1;

    for (std::size_t i{ 0 }; i < 3; ++i)
        for (std::size_t j{ 0 }; j < 3; ++j)
            EXPECT_EQ(expected1[i][j], actual1[i][j]);

    for (std::size_t i{ 0 }; i < 3; ++i)
        for (std::size_t j{ 0 }; j < 3; ++j)
            EXPECT_EQ(expected2[i][j], actual2[i][j]);
}

TEST(Matrix3x3Test, BinaryMultiplicationOperator_MatrixWithVector)
{
    auto sut1 = Zeus::Math::Matrix3x3(
        // clang-format off
        1.f, 4.f, 7.f,
        2.f, 5.f, 8.f,
        3.f, 6.f, 9.f
        // clang-format on
    );

    auto sut2 = Zeus::Math::Vector3(2.f, 4.f, 8.f);

    auto expected = Zeus::Math::Vector3(34.f, 76.f, 118.f);

    auto actual = sut1 * sut2;

    for (std::size_t i{ 0 }; i < 3; ++i)
        EXPECT_EQ(expected[i], actual[i]);
}
