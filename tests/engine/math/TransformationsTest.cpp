#include <math/Matrix3x3.hpp>
#include <math/Vector3.hpp>
#include <math/transformations.hpp>

#include <gtest/gtest.h>

TEST(TransformationsTest, Identity_Matrix3x3)
{
    auto identity = Zeus::identity<Zeus::Matrix3x3<float>>();

    auto sut = Zeus::Matrix3x3(
        // clang-format off
        1.f, 1.f, 1.f,
        1.f, 1.f, 1.f,
        1.f, 1.f, 1.f
        // clang-format on
    );

    auto actual = identity * sut;

    for (std::size_t i{ 0 }; i < 3; ++i)
        for (std::size_t j{ 0 }; j < 3; ++j)
            EXPECT_EQ(sut[i][j], actual[i][j]);
}

TEST(TransformationsTest, Identity_Matrix4x4)
{
    auto identity = Zeus::identity<Zeus::Matrix4x4<float>>();

    auto sut = Zeus::Matrix4x4(
        // clang-format off
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f
        // clang-format on
    );

    auto actual = identity * sut;

    for (std::size_t i{ 0 }; i < 4; ++i)
        for (std::size_t j{ 0 }; j < 4; ++j)
            EXPECT_EQ(sut[i][j], actual[i][j]);
}

TEST(TransformationsTest, ReflectionOrigin_Matrix4x4)
{
    auto reflection = Zeus::reflectionOrigin<float>();

    auto sut = Zeus::Matrix4x4(
        // clang-format off
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f
        // clang-format on
    );

    auto expected = Zeus::Matrix4x4(
        // clang-format off
        -1.f, -1.f, -1.f, 1.f,
        -1.f, -1.f, -1.f, 1.f,
        -1.f, -1.f, -1.f, 1.f,
        -1.f, -1.f, -1.f, 1.f
        // clang-format on
    );

    auto actual = reflection * sut;

    for (std::size_t i{ 0 }; i < 4; ++i)
        for (std::size_t j{ 0 }; j < 4; ++j)
            EXPECT_EQ(expected[i][j], actual[i][j]);
}

TEST(TransformationsTest, ReflectionX_Matrix4x4)
{
    auto reflection = Zeus::reflectionX<float>();

    auto sut = Zeus::Matrix4x4(
        // clang-format off
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f
        // clang-format on
    );

    auto expected = Zeus::Matrix4x4(
        // clang-format off
        -1.f, 1.f, 1.f, 1.f,
        -1.f, 1.f, 1.f, 1.f,
        -1.f, 1.f, 1.f, 1.f,
        -1.f, 1.f, 1.f, 1.f
        // clang-format on
    );

    auto actual = reflection * sut;

    for (std::size_t i{ 0 }; i < 4; ++i)
        for (std::size_t j{ 0 }; j < 4; ++j)
            EXPECT_EQ(expected[i][j], actual[i][j]);
}

TEST(TransformationsTest, ReflectionY_Matrix4x4)
{
    auto reflection = Zeus::reflectionY<float>();

    auto sut = Zeus::Matrix4x4(
        // clang-format off
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f
        // clang-format on
    );

    auto expected = Zeus::Matrix4x4(
        // clang-format off
        1.f, -1.f, 1.f, 1.f,
        1.f, -1.f, 1.f, 1.f,
        1.f, -1.f, 1.f, 1.f,
        1.f, -1.f, 1.f, 1.f
        // clang-format on
    );

    auto actual = reflection * sut;

    for (std::size_t i{ 0 }; i < 4; ++i)
        for (std::size_t j{ 0 }; j < 4; ++j)
            EXPECT_EQ(expected[i][j], actual[i][j]);
}

TEST(TransformationsTest, Translation_Matrix4x4)
{
    auto translation = Zeus::translation(Zeus::Vector3<float>(1.f, 2.f, 3.f));

    auto sut = Zeus::Matrix4x4(
        // clang-format off
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f
        // clang-format on
    );

    auto expected = Zeus::Matrix4x4(
        // clang-format off
        2.f, 3.f, 4.f, 1.f,
        2.f, 3.f, 4.f, 1.f,
        2.f, 3.f, 4.f, 1.f,
        2.f, 3.f, 4.f, 1.f
        // clang-format on
    );

    auto actual = translation * sut;

    for (std::size_t i{ 0 }; i < 4; ++i)
        for (std::size_t j{ 0 }; j < 4; ++j)
            EXPECT_EQ(expected[i][j], actual[i][j]);
}

TEST(TransformationsTest, Scale_Matrix4x4_Scalar)
{
    auto scale = Zeus::scale(2.f);

    auto sut = Zeus::Matrix4x4(
        // clang-format off
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f
        // clang-format on
    );

    auto expected = Zeus::Matrix4x4(
        // clang-format off
        2.f, 2.f, 2.f, 1.f,
        2.f, 2.f, 2.f, 1.f,
        2.f, 2.f, 2.f, 1.f,
        2.f, 2.f, 2.f, 1.f
        // clang-format on
    );

    auto actual = scale * sut;

    for (std::size_t i{ 0 }; i < 4; ++i)
        for (std::size_t j{ 0 }; j < 4; ++j)
            EXPECT_EQ(expected[i][j], actual[i][j]);
}

TEST(TransformationsTest, Scale_Matrix4x4_Vector)
{
    auto scale = Zeus::scale(Zeus::Vector3<float>(2.f, 3.f, 4.f));

    auto sut = Zeus::Matrix4x4(
        // clang-format off
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f
        // clang-format on
    );

    auto expected = Zeus::Matrix4x4(
        // clang-format off
        2.f, 3.f, 4.f, 1.f,
        2.f, 3.f, 4.f, 1.f,
        2.f, 3.f, 4.f, 1.f,
        2.f, 3.f, 4.f, 1.f
        // clang-format on
    );

    auto actual = scale * sut;

    for (std::size_t i{ 0 }; i < 4; ++i)
        for (std::size_t j{ 0 }; j < 4; ++j)
            EXPECT_EQ(expected[i][j], actual[i][j]);
}
