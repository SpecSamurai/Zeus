#include <math/Matrix3x3.hpp>
#include <math/Quaternion.hpp>
#include <math/Vector3.hpp>
#include <math/definitions.hpp>
#include <math/transformations.hpp>

#include <gtest/gtest.h>

#include <cstdint>

TEST(TransformationsTest, toUNormInt)
{
    std::uint32_t sut1 = Zeus::Math::toUNormInt(Zeus::Math::Color(1, 1, 1, 1));
    std::uint32_t sut2 = Zeus::Math::toUNormInt(Zeus::Math::Color(0, 0, 0, 0));
    std::uint32_t sut3 = Zeus::Math::toUNormInt(Zeus::Math::Color(0, 1, 1, 1));
    std::uint32_t sut4 = Zeus::Math::toUNormInt(Zeus::Math::Color(0, 0, 1, 1));
    std::uint32_t sut5 = Zeus::Math::toUNormInt(Zeus::Math::Color(0, 0, 0, 1));
    std::uint32_t sut6 = Zeus::Math::toUNormInt(Zeus::Math::Color(0, 0, 1, 0));
    std::uint32_t sut7 = Zeus::Math::toUNormInt(Zeus::Math::Color(0, 1, 0, 0));
    std::uint32_t sut8 = Zeus::Math::toUNormInt(Zeus::Math::Color(1, 0, 0, 0));

    EXPECT_EQ(sut1, 0xffffffff);
    EXPECT_EQ(sut2, 0x00000000);
    EXPECT_EQ(sut3, 0xffffff00);
    EXPECT_EQ(sut4, 0xffff0000);
    EXPECT_EQ(sut5, 0xff000000);
    EXPECT_EQ(sut6, 0x00ff0000);
    EXPECT_EQ(sut7, 0x0000ff00);
    EXPECT_EQ(sut8, 0x000000ff);
}

TEST(TransformationsTest, transpose_Matrix3x3)
{
    auto sut = Zeus::Math::Matrix3x3(
        // clang-format off
        1.f, 2.f, 3.f,
        4.f, 5.f, 6.f,
        7.f, 8.f, 9.f
        // clang-format on
    );

    auto expected = Zeus::Math::Matrix3x3(
        // clang-format off
        1.f, 4.f, 7.f,
        2.f, 5.f, 8.f,
        3.f, 6.f, 9.f
        // clang-format on
    );

    Zeus::Math::transpose(sut);

    for (std::size_t i{ 0 }; i < 3; ++i)
        for (std::size_t j{ 0 }; j < 3; ++j)
            EXPECT_EQ(sut[i][j], expected[i][j]);
}

TEST(TransformationsTest, transpose_Matrix4x4)
{
    auto sut = Zeus::Math::Matrix4x4(
        // clang-format off
        1.f, 2.f, 3.f, 4.f,
        5.f, 6.f, 7.f, 8.f,
        9.f, 10.f, 11.f, 12.f,
        13.f, 14.f, 15.f, 16.f
        // clang-format on
    );

    auto expected = Zeus::Math::Matrix4x4(
        // clang-format off
        1.f, 5.f, 9.f, 13.f,
        2.f, 6.f, 10.f, 14.f,
        3.f, 7.f, 11.f, 15.f,
        4.f, 8.f, 12.f, 16.f
        // clang-format on
    );

    Zeus::Math::transpose(sut);

    for (std::size_t i{ 0 }; i < 4; ++i)
        for (std::size_t j{ 0 }; j < 4; ++j)
            EXPECT_EQ(sut[i][j], expected[i][j]);
}

TEST(TransformationsTest, Identity_Matrix3x3)
{
    auto identity = Zeus::Math::identity<Zeus::Math::Matrix3x3<float>>();

    auto sut = Zeus::Math::Matrix3x3(
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
    auto identity = Zeus::Math::identity<Zeus::Math::Matrix4x4<float>>();

    auto sut = Zeus::Math::Matrix4x4(
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
    auto reflection = Zeus::Math::reflectionOrigin<float>();

    auto sut = Zeus::Math::Matrix4x4(
        // clang-format off
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f
        // clang-format on
    );

    auto expected = Zeus::Math::Matrix4x4(
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
    auto reflection = Zeus::Math::reflectionX<float>();

    auto sut = Zeus::Math::Matrix4x4(
        // clang-format off
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f
        // clang-format on
    );

    auto expected = Zeus::Math::Matrix4x4(
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
    auto reflection = Zeus::Math::reflectionY<float>();

    auto sut = Zeus::Math::Matrix4x4(
        // clang-format off
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f
        // clang-format on
    );

    auto expected = Zeus::Math::Matrix4x4(
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
    auto translation =
        Zeus::Math::translation(Zeus::Math::Vector3<float>(1.f, 2.f, 3.f));

    auto sut = Zeus::Math::Matrix4x4(
        // clang-format off
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f
        // clang-format on
    );

    auto expected = Zeus::Math::Matrix4x4(
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
    auto scale = Zeus::Math::uniformScale(2.f);

    auto sut = Zeus::Math::Matrix4x4(
        // clang-format off
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f
        // clang-format on
    );

    auto expected = Zeus::Math::Matrix4x4(
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
    auto scale = Zeus::Math::scale(Zeus::Math::Vector3<float>(2.f, 3.f, 4.f));

    auto sut = Zeus::Math::Matrix4x4(
        // clang-format off
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f,
        1.f, 1.f, 1.f, 1.f
        // clang-format on
    );

    auto expected = Zeus::Math::Matrix4x4(
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

TEST(TransformationsTest, Quaternion_rotateVector)
{
    auto sut = Zeus::Math::Quaternion(1.f, 2.f, 3.f, 4.f);
    auto vector = Zeus::Math::Vector3(1.f, 1.f, 1.f);

    auto actual = Zeus::Math::rotateVector(sut, vector);

    auto expected = Zeus::Math::Vector3(-23, 13, 1);

    EXPECT_EQ(expected.x, actual.x);
    EXPECT_EQ(expected.y, actual.y);
    EXPECT_EQ(expected.z, actual.z);
}

TEST(TransformationsTest, Quaternion_rotationMatrix3x3)
{
    auto sut = Zeus::Math::Quaternion(1.f, 2.f, 3.f, 4.f);

    auto actual = Zeus::Math::rotationMatrix3x3(sut);

    auto expected = Zeus::Math::Matrix3x3<float>(
        // clang-format off
        -25.0f, 28.0f, -10.0f,
        -20.0f, -19.0f, 20.0f,
        22.0f,  4.0f,  -9.0f
        // clang-format on
    );

    for (std::size_t i{ 0 }; i < 3; ++i)
        for (std::size_t j{ 0 }; j < 3; ++j)
            EXPECT_EQ(expected[i][j], actual[i][j]);
}

TEST(TransformationsTest, Quaternion_rotationMatrix4x4)
{
    auto sut = Zeus::Math::Quaternion(1.f, 2.f, 3.f, 4.f);

    auto actual = Zeus::Math::rotationMatrix4x4(sut);

    auto expected = Zeus::Math::Matrix4x4<float>(
        // clang-format off
        -25.0f, 28.0f, -10.0f, 0.f,
        -20.0f, -19.0f, 20.0f, 0.f,
        22.0f,  4.0f,  -9.0f, 0.f,
        0.f, 0.f, 0.f, 1.f
        // clang-format on
    );

    for (std::size_t i{ 0 }; i < 4; ++i)
        for (std::size_t j{ 0 }; j < 4; ++j)
            EXPECT_EQ(expected[i][j], actual[i][j]);
}
