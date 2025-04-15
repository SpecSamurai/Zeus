#include <core/Hasher.hpp>

#include <gtest/gtest.h>

enum class TestEnum
{
    One,
    Two,
};

struct TestStruct
{
    int value;
    TestEnum testEnum;
};

TEST(HasherTest, PrimitiveTypes_Digest_Equal)
{
    auto actual1 = Zeus::Hasher::Digest(1);

    int value = 1;
    auto actual2 = Zeus::Hasher::Digest(value);

    EXPECT_EQ(actual1, actual2);
}

TEST(HasherTest, PrimitiveTypes_Digest_NotEqual)
{
    auto actual1 = Zeus::Hasher::Digest(1);

    int value = 2;
    auto actual2 = Zeus::Hasher::Digest(value);

    EXPECT_NE(actual1, actual2);
}

TEST(HasherTest, PrimitiveTypes_CombineSingle_Equal)
{
    Zeus::Hasher sut1;
    Zeus::Hasher sut2;

    sut1.Combine(1);

    int value = 1;
    sut2.Combine(value);

    auto actual1 = sut1.Digest();
    auto actual2 = sut2.Digest();

    EXPECT_EQ(actual1, actual2);
}

TEST(HasherTest, PrimitiveTypes_CombineSingle_NotEqual)
{
    Zeus::Hasher sut1;
    Zeus::Hasher sut2;

    sut1.Combine(1);

    int value = 2;
    sut2.Combine(value);

    auto actual1 = sut1.Digest();
    auto actual2 = sut2.Digest();

    EXPECT_NE(actual1, actual2);
}

TEST(HasherTest, ComplexTypes_CombineMultiple_Equal)
{
    Zeus::Hasher sut1;
    Zeus::Hasher sut2;

    sut1.Combine(1);
    sut1.Combine(TestEnum::One);

    TestStruct test{
        .value = 1,
        .testEnum = TestEnum::One,
    };
    sut2.Combine(test.value);
    sut2.Combine(test.testEnum);

    auto actual1 = sut1.Digest();
    auto actual2 = sut2.Digest();

    EXPECT_EQ(actual1, actual2);
}
