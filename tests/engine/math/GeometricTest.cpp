#include <math/Vector3.hpp>
#include <math/Vector4.hpp>
#include <math/geometric.hpp>

#include <gtest/gtest.h>

TEST(GeometricTest, DotProduct_Vector2_Properties)
{
    Zeus::Math::Vector2 v1(1.f, 2.f);
    Zeus::Math::Vector2 v2(5.f, 6.f);

    EXPECT_EQ(Zeus::Math::dot(v1, v2), Zeus::Math::dot(v2, v1));
}

TEST(GeometricTest, DotProduct_Vector3_Properties)
{
    Zeus::Math::Vector3 v1(1.f, 2.f, 3.f);
    Zeus::Math::Vector3 v2(5.f, 6.f, 7.f);

    EXPECT_EQ(Zeus::Math::dot(v1, v2), Zeus::Math::dot(v2, v1));
}

TEST(GeometricTest, DotProduct_Vector4_Properties)
{
    Zeus::Math::Vector4 v1(1.f, 2.f, 3.f, 4.f);
    Zeus::Math::Vector4 v2(5.f, 6.f, 7.f, 8.f);

    EXPECT_EQ(Zeus::Math::dot(v1, v2), Zeus::Math::dot(v2, v1));
}

TEST(GeometricTest, DotProduct_Vector2_Orthogonality)
{
    Zeus::Math::Vector2 v1(1.f, 2.f);
    Zeus::Math::Vector2 v2(2.f, -1.f);

    EXPECT_EQ(0.f, Zeus::Math::dot(v1, v2));
}

TEST(GeometricTest, DotProduct_Vector3_Orthogonality)
{
    Zeus::Math::Vector3 v1(1.f, 2.f, 3.f);
    Zeus::Math::Vector3 v2(4.f, -8.f, 4.f);

    EXPECT_EQ(0.f, Zeus::Math::dot(v1, v2));
}

TEST(GeometricTest, DotProduct_Vector4_Orthogonality)
{
    Zeus::Math::Vector4 v1(1.f, 2.f, 3.f, 4.f);
    Zeus::Math::Vector4 v2(1.f, -2.f, 1.f, 0.f);

    EXPECT_EQ(0.f, Zeus::Math::dot(v1, v2));
}

TEST(GeometricTest, DotProduct_Quaternion_Properties)
{
    Zeus::Math::Quaternion v1(1.f, 2.f, 3.f, 4.f);
    Zeus::Math::Quaternion v2(5.f, 6.f, 7.f, 8.f);

    EXPECT_EQ(Zeus::Math::dot(v1, v2), Zeus::Math::dot(v2, v1));
}

TEST(GeometricTest, CrossProduct_Vector3)
{
    Zeus::Math::Vector3 v1(1.f, 0.f, 0.f);
    Zeus::Math::Vector3 v2(0.f, 1.f, 0.f);

    auto actual = Zeus::Math::cross(v1, v2);
    auto expected = Zeus::Math::Vector3(0.f, 0.f, 1.f);

    EXPECT_EQ(expected.x, actual.x);
    EXPECT_EQ(expected.y, actual.y);
    EXPECT_EQ(expected.z, actual.z);
}

TEST(GeometricTest, CrossProduct_Vector4)
{
    Zeus::Math::Vector4 v1(1.f, 0.f, 0.f, 1.f);
    Zeus::Math::Vector4 v2(0.f, 1.f, 0.f, 1.f);

    auto actual = Zeus::Math::cross(v1, v2);
    auto expected = Zeus::Math::Vector4(0.f, 0.f, 1.f, 0.f);

    EXPECT_EQ(expected.x, actual.x);
    EXPECT_EQ(expected.y, actual.y);
    EXPECT_EQ(expected.z, actual.z);
    EXPECT_EQ(expected.w, actual.w);
}

TEST(GeometricTest, Length_Vector2)
{
    Zeus::Math::Vector2 sut(3.f, 4.f);

    auto actual = Zeus::Math::length(sut);

    EXPECT_EQ(5.f, actual);
}

TEST(GeometricTest, Length_Vector3)
{
    Zeus::Math::Vector3 sut(1.f, 2.f, 2.f);

    auto actual = Zeus::Math::length(sut);

    EXPECT_EQ(3.f, actual);
}

TEST(GeometricTest, Length_Vector4)
{
    Zeus::Math::Vector4 sut(1.f, 2.f, 2.f, 4.f);

    auto actual = Zeus::Math::length(sut);

    EXPECT_EQ(5.f, actual);
}

TEST(GeometricTest, min_Vector2)
{
    Zeus::Math::Vector2 sut1(1.f, 4.f);
    Zeus::Math::Vector2 sut2(2.f, 3.f);

    auto actual = Zeus::Math::min(sut1, sut2);

    EXPECT_EQ(1.f, actual.x);
    EXPECT_EQ(3.f, actual.y);
}

TEST(GeometricTest, min_Vector3)
{
    Zeus::Math::Vector3 sut1(1.f, 4.f, 5.f);
    Zeus::Math::Vector3 sut2(2.f, 3.f, 6.f);

    auto actual = Zeus::Math::min(sut1, sut2);

    EXPECT_EQ(1.f, actual.x);
    EXPECT_EQ(3.f, actual.y);
    EXPECT_EQ(5.f, actual.z);
}

TEST(GeometricTest, min_Vector4)
{
    Zeus::Math::Vector4 sut1(1.f, 4.f, 5.f, 8.f);
    Zeus::Math::Vector4 sut2(2.f, 3.f, 6.f, 7.f);

    auto actual = Zeus::Math::min(sut1, sut2);

    EXPECT_EQ(1.f, actual.x);
    EXPECT_EQ(3.f, actual.y);
    EXPECT_EQ(5.f, actual.z);
    EXPECT_EQ(7.f, actual.w);
}

TEST(GeometricTest, max_Vector2)
{
    Zeus::Math::Vector2 sut1(1.f, 4.f);
    Zeus::Math::Vector2 sut2(2.f, 3.f);

    auto actual = Zeus::Math::max(sut1, sut2);

    EXPECT_EQ(2.f, actual.x);
    EXPECT_EQ(4.f, actual.y);
}

TEST(GeometricTest, max_Vector3)
{
    Zeus::Math::Vector3 sut1(1.f, 4.f, 5.f);
    Zeus::Math::Vector3 sut2(2.f, 3.f, 6.f);

    auto actual = Zeus::Math::max(sut1, sut2);

    EXPECT_EQ(2.f, actual.x);
    EXPECT_EQ(4.f, actual.y);
    EXPECT_EQ(6.f, actual.z);
}

TEST(GeometricTest, max_Vector4)
{
    Zeus::Math::Vector4 sut1(1.f, 4.f, 5.f, 8.f);
    Zeus::Math::Vector4 sut2(2.f, 3.f, 6.f, 7.f);

    auto actual = Zeus::Math::max(sut1, sut2);

    EXPECT_EQ(2.f, actual.x);
    EXPECT_EQ(4.f, actual.y);
    EXPECT_EQ(6.f, actual.z);
    EXPECT_EQ(8.f, actual.w);
}

TEST(GeometricTest, norm_Quaternion)
{
    Zeus::Math::Quaternion sut(1.f, 2.f, 2.f, 4.f);

    auto actual = Zeus::Math::norm(sut);

    EXPECT_EQ(5.f, actual);
}

TEST(GeometricTest, Normalize_Vector2)
{
    Zeus::Math::Vector2 sut(3.f, 4.f);
    Zeus::Math::Vector2 expected(0.6f, 0.8f);

    auto actual = Zeus::Math::normalize(sut);

    EXPECT_EQ(expected.x, actual.x);
    EXPECT_EQ(expected.y, actual.y);
}

TEST(GeometricTest, Normalize_Vector3)
{
    Zeus::Math::Vector3 sut(1.f, 2.f, 2.f);
    Zeus::Math::Vector3 expected(1.f / 3.f, 2.f / 3.f, 2.f / 3.f);

    auto actual = Zeus::Math::normalize(sut);

    EXPECT_EQ(expected.x, actual.x);
    EXPECT_EQ(expected.y, actual.y);
    EXPECT_EQ(expected.z, actual.z);
}

TEST(GeometricTest, Normalize_Vector4)
{
    Zeus::Math::Vector4 sut(1.f, 2.f, 2.f, 4.f);
    Zeus::Math::Vector4 expected(1.f / 5.f, 2.f / 5.f, 2.f / 5.f, 4.f / 5.f);

    auto actual = Zeus::Math::normalize(sut);

    EXPECT_EQ(expected.x, actual.x);
    EXPECT_EQ(expected.y, actual.y);
    EXPECT_EQ(expected.z, actual.z);
    EXPECT_EQ(expected.w, actual.w);
}

TEST(GeometricTest, Normalize_Quaternion)
{
    Zeus::Math::Quaternion sut(1.f, 2.f, 2.f, 4.f);
    Zeus::Math::Quaternion expected(1.f / 5.f, 2.f / 5.f, 2.f / 5.f, 4.f / 5.f);

    auto actual = Zeus::Math::normalize(sut);

    EXPECT_EQ(expected.x, actual.x);
    EXPECT_EQ(expected.y, actual.y);
    EXPECT_EQ(expected.z, actual.z);
    EXPECT_EQ(expected.w, actual.w);
}

TEST(GeometricTest, Normalize_conjugate)
{
    Zeus::Math::Quaternion sut(1.f, 2.f, 2.f, 4.f);

    auto actual = Zeus::Math::conjugate(sut);

    EXPECT_EQ(-1.f, actual.x);
    EXPECT_EQ(-2.f, actual.y);
    EXPECT_EQ(-2.f, actual.z);
    EXPECT_EQ(4.f, actual.w);
}

TEST(GeometricTest, Normalize_inverse)
{
    Zeus::Math::Quaternion sut(1.f, 2.f, 2.f, 4.f);

    auto actual = Zeus::Math::inverse(sut);

    EXPECT_EQ(-1.f / 25.f, actual.x);
    EXPECT_EQ(-2.f / 25.f, actual.y);
    EXPECT_EQ(-2.f / 25.f, actual.z);
    EXPECT_EQ(4.f / 25.f, actual.w);
}
