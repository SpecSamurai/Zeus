#include <math/Matrix4x4.hpp>

#include <gtest/gtest.h>

TEST(Matrix4x4Test, Constructor_ArraySubscriptOperator)
{
    auto sut = Zeus::Math::Matrix4x4(
        // clang-format off
        0.f, 0.f, 0.f, 0.f,
        0.f, 0.f, 0.f, 0.f,
        0.f, 0.f, 0.f, 0.f,
        0.f, 0.f, 0.f, 0.f
        // clang-format on
    );

    for (std::size_t i{ 0 }; i < 4; ++i)
        for (std::size_t j{ 0 }; j < 4; ++j)
            EXPECT_EQ(0.f, sut[i][j]);

    float temp{ 1.f };
    for (std::size_t i{ 0 }; i < 4; ++i)
        for (std::size_t j{ 0 }; j < 4; ++j)
            sut[i][j] = temp++;

    temp = 1.f;
    for (std::size_t i{ 0 }; i < 4; ++i)
        for (std::size_t j{ 0 }; j < 4; ++j)
            EXPECT_EQ(temp++, sut[i][j]);
}

TEST(Matrix4x4Test, Constructor_Scalar)
{
    auto sut = Zeus::Math::Matrix4x4(1.f);

    for (std::size_t i{ 0 }; i < 4; ++i)
        for (std::size_t j{ 0 }; j < 4; ++j)
            if (i == j)
                EXPECT_EQ(1.f, sut[i][j]);
            else
                EXPECT_EQ(0, sut[i][j]);
}

TEST(Matrix4x4Test, BinaryAdditionOperator_Matrices)
{
    auto sut1 = Zeus::Math::Matrix4x4(
        // clang-format off
        1.f, 2.f, 3.f, 4.f,
        5.f, 6.f, 7.f, 8.f,
        9.f, 10.f, 11.f, 12.f,
        13.f, 14.f, 15.f, 16.f
        // clang-format on
    );

    auto sut2 = Zeus::Math::Matrix4x4(
        // clang-format off
        10.f, 11.f, 12.f, 13.f,
        14.f, 15.f, 16.f, 17.f,
        18.f, 19.f, 20.f, 21.f,
        22.f, 23.f, 24.f, 25.f
        // clang-format on
    );

    auto expected = Zeus::Math::Matrix4x4(
        // clang-format off
        11.f, 13.f, 15.f, 17.f,
        19.f, 21.f, 23.f, 25.f,
        27.f, 29.f, 31.f, 33.f,
        35.f, 37.f, 39.f, 41.f
        // clang-format on
    );

    auto actual = sut1 + sut2;

    for (std::size_t i{ 0 }; i < 4; ++i)
        for (std::size_t j{ 0 }; j < 4; ++j)
            EXPECT_EQ(expected[i][j], actual[i][j]);
}

TEST(Matrix4x4Test, BinaryAdditionOperator_MatrixWithScalar)
{
    auto sut = Zeus::Math::Matrix4x4(
        // clang-format off
        1.f, 2.f, 3.f, 4.f,
        5.f, 6.f, 7.f, 8.f,
        9.f, 10.f, 11.f, 12.f,
        13.f, 14.f, 15.f, 16.f
        // clang-format on
    );

    float scalar = 1.f;

    auto expected = Zeus::Math::Matrix4x4(
        // clang-format off
        2.f, 3.f, 4.f, 5.f,
        6.f, 7.f, 8.f, 9.f,
        10.f, 11.f, 12.f, 13.f,
        14.f, 15.f, 16.f, 17.f
        // clang-format on
    );

    auto actual1 = scalar + sut;
    auto actual2 = sut + scalar;

    for (std::size_t i{ 0 }; i < 4; ++i)
        for (std::size_t j{ 0 }; j < 4; ++j)
            EXPECT_EQ(expected[i][j], actual1[i][j]);

    for (std::size_t i{ 0 }; i < 4; ++i)
        for (std::size_t j{ 0 }; j < 4; ++j)
            EXPECT_EQ(expected[i][j], actual2[i][j]);
}

TEST(Matrix4x4Test, BinarySubtractionOperator_Matrices)
{
    auto sut1 = Zeus::Math::Matrix4x4(
        // clang-format off
        16.f, 15.f, 14.f, 13.f,
        12.f, 11.f, 10.f, 9.f,
        8.f, 7.f, 6.f, 5.f,
        4.f, 3.f, 2.f, 1.f
        // clang-format on
    );

    auto sut2 = Zeus::Math::Matrix4x4(
        // clang-format off
        1.f, 2.f, 3.f, 4.f,
        5.f, 6.f, 7.f, 8.f,
        9.f, 10.f, 11.f, 12.f,
        13.f, 14.f, 15.f, 16.f
        // clang-format on
    );

    auto expected1 = Zeus::Math::Matrix4x4(
        // clang-format off
        15.f, 13.f, 11.f, 9.f,
        7.f, 5.f, 3.f, 1.f,
        -1.f, -3.f, -5.f, -7.f,
        -9.f, -11.f, -13.f, -15.f
        // clang-format on
    );

    auto expected2 = Zeus::Math::Matrix4x4(
        // clang-format off
        -15.f, -13.f, -11.f, -9.f,
        -7.f, -5.f, -3.f, -1.f,
        1.f,3.f, 5.f, 7.f,
        9.f, 11.f, 13.f, 15.f
        // clang-format on
    );

    auto actual1 = sut1 - sut2;
    auto actual2 = sut2 - sut1;

    for (std::size_t i{ 0 }; i < 4; ++i)
        for (std::size_t j{ 0 }; j < 4; ++j)
            EXPECT_EQ(expected1[i][j], actual1[i][j]);

    for (std::size_t i{ 0 }; i < 4; ++i)
        for (std::size_t j{ 0 }; j < 4; ++j)
            EXPECT_EQ(expected2[i][j], actual2[i][j]);
}

TEST(Matrix4x4Test, BinarySubtractionOperator_MatrixWithScalar)
{
    auto sut = Zeus::Math::Matrix4x4(
        // clang-format off
        1.f, 2.f, 3.f, 4.f,
        5.f, 6.f, 7.f, 8.f,
        9.f, 10.f, 11.f, 12.f,
        13.f, 14.f, 15.f, 16.f
        // clang-format on
    );

    float scalar = 1.f;

    auto expected1 = Zeus::Math::Matrix4x4(
        // clang-format off
        0.f, -1.f, -2.f, -3.f,
        -4.f, -5.f, -6.f, -7.f,
        -8.f, -9.f, -10.f, -11.f,
        -12.f, -13.f, -14.f, -15.f
        // clang-format on
    );

    auto expected2 = Zeus::Math::Matrix4x4(
        // clang-format off
        0.f, 1.f, 2.f, 3.f,
        4.f, 5.f, 6.f, 7.f,
        8.f, 9.f, 10.f, 11.f,
        12.f, 13.f, 14.f, 15.f
        // clang-format on
    );

    auto actual1 = scalar - sut;
    auto actual2 = sut - scalar;

    for (std::size_t i{ 0 }; i < 4; ++i)
        for (std::size_t j{ 0 }; j < 4; ++j)
            EXPECT_EQ(expected1[i][j], actual1[i][j]);

    for (std::size_t i{ 0 }; i < 4; ++i)
        for (std::size_t j{ 0 }; j < 4; ++j)
            EXPECT_EQ(expected2[i][j], actual2[i][j]);
}

TEST(Matrix4x4Test, BinaryMultiplicationOperator_MatrixWithScalar)
{
    auto sut = Zeus::Math::Matrix4x4(
        // clang-format off
        1.f, 2.f, 3.f, 4.f,
        5.f, 6.f, 7.f, 8.f,
        9.f, 10.f, 11.f, 12.f,
        13.f, 14.f, 15.f, 16.f
        // clang-format on
    );

    float scalar = 2.f;

    auto expected = Zeus::Math::Matrix4x4(
        // clang-format off
        2.f, 4.f, 6.f, 8.f,
        10.f, 12.f, 14.f, 16.f,
        18.f, 20.f, 22.f, 24.f,
        26.f, 28.f, 30.f, 32.f
        // clang-format on
    );

    auto actual1 = scalar * sut;
    auto actual2 = sut * scalar;

    for (std::size_t i{ 0 }; i < 4; ++i)
        for (std::size_t j{ 0 }; j < 4; ++j)
            EXPECT_EQ(expected[i][j], actual1[i][j]);

    for (std::size_t i{ 0 }; i < 4; ++i)
        for (std::size_t j{ 0 }; j < 4; ++j)
            EXPECT_EQ(expected[i][j], actual2[i][j]);
}

TEST(Matrix4x4Test, BinaryDivisionOperator_MatrixWithScalar)
{
    auto sut = Zeus::Math::Matrix4x4(
        // clang-format off
        2.f, 4.f, 6.f, 8.f,
        10.f, 12.f, 14.f, 16.f,
        18.f, 20.f, 22.f, 24.f,
        26.f, 28.f, 30.f, 32.f
        // clang-format on
    );

    float scalar = 2.f;

    auto expected1 = Zeus::Math::Matrix4x4(
        // clang-format off
        2.f / 2.f, 2.f / 4.f, 2.f / 6.f, 2.f / 8.f,
        2.f / 10.f, 2.f / 12.f, 2.f / 14.f, 2.f / 16.f,
        2.f / 18.f, 2.f / 20.f, 2.f / 22.f, 2.f / 24.f,
        2.f / 26.f, 2.f / 28.f, 2.f / 30.f, 2.f / 32.f
        // clang-format on
    );

    auto expected2 = Zeus::Math::Matrix4x4(
        // clang-format off
        1.f, 2.f, 3.f, 4.f,
        5.f, 6.f, 7.f, 8.f,
        9.f, 10.f, 11.f, 12.f,
        13.f, 14.f, 15.f, 16.f
        // clang-format on
    );

    auto actual1 = scalar / sut;
    auto actual2 = sut / scalar;

    for (std::size_t i{ 0 }; i < 4; ++i)
        for (std::size_t j{ 0 }; j < 4; ++j)
            EXPECT_EQ(expected1[i][j], actual1[i][j]);

    for (std::size_t i{ 0 }; i < 4; ++i)
        for (std::size_t j{ 0 }; j < 4; ++j)
            EXPECT_EQ(expected2[i][j], actual2[i][j]);
}

TEST(Matrix4x4Test, BinaryMultiplicationOperator_MatrixWithMatrix)
{
    auto sut1 = Zeus::Math::Matrix4x4(
        // clang-format off
        1.f, 5.f, 9.f, 13.f,
        2.f, 6.f, 10.f, 14.f,
        3.f, 7.f, 11.f, 15.f,
        4.f, 8.f, 12.f, 16.f
        // clang-format on
    );

    auto sut2 = Zeus::Math::Matrix4x4(
        // clang-format off
        10.f, 14.f, 18.f, 22.f,
        11.f, 15.f, 19.f, 23.f,
        12.f, 16.f, 20.f, 24.f,
        13.f, 17.f, 21.f, 25.f
        // clang-format on
    );

    auto expected1 = Zeus::Math::Matrix4x4(
        // clang-format off
        180.f, 436.f, 692.f, 948.f,
        190.f, 462.f, 734.f, 1006.f,
        200.f, 488.f, 776.f, 1064.f,
        210.f, 514.f, 818.f, 1122.f
        // clang-format on
    );

    auto expected2 = Zeus::Math::Matrix4x4(
        // clang-format off
        342.f, 454.f, 566.f, 678.f,
        388.f, 516.f, 644.f, 772.f,
        434.f, 578.f, 722.f, 866.f,
        480.f, 640.f, 800.f, 960.f
        // clang-format on
    );

    auto actual1 = sut1 * sut2;
    auto actual2 = sut2 * sut1;

    for (std::size_t i{ 0 }; i < 4; ++i)
        for (std::size_t j{ 0 }; j < 4; ++j)
            EXPECT_EQ(expected1[i][j], actual1[i][j]);

    for (std::size_t i{ 0 }; i < 4; ++i)
        for (std::size_t j{ 0 }; j < 4; ++j)
            EXPECT_EQ(expected2[i][j], actual2[i][j]);
}

TEST(Matrix4x4Test, BinaryMultiplicationOperator_MatrixWithVector)
{
    auto sut1 = Zeus::Math::Matrix4x4(
        // clang-format off
        1.f, 5.f, 9.f, 13.f,
        2.f, 6.f, 10.f, 14.f,
        3.f, 7.f, 11.f, 15.f,
        4.f, 8.f, 12.f, 16.f
        // clang-format on
    );

    auto sut2 = Zeus::Math::Vector4(2.f, 4.f, 8.f, 16.f);

    auto expected = Zeus::Math::Vector4(98.f, 218.f, 338.f, 458.f);

    auto actual = sut1 * sut2;

    for (std::size_t i{ 0 }; i < 4; ++i)
        EXPECT_EQ(expected[i], actual[i]);
}
