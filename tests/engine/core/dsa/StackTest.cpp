#include <core/dsa/Stack.hpp>

#include "gtest/gtest.h"

TEST(StackTest, Push)
{
    auto sut = Zeus::Stack<int>();

    sut.Push(1);
    EXPECT_EQ(1, sut.Peek());
    EXPECT_EQ(1, sut.Size());

    sut.Push(10);
    EXPECT_EQ(10, sut.Peek());
    EXPECT_EQ(2, sut.Size());
}

TEST(StackTest, Pop)
{
    auto sut = Zeus::Stack<int>();
    sut.Push(1);
    sut.Push(2);

    EXPECT_EQ(2, sut.Pop());
    EXPECT_EQ(1, sut.Size());
    EXPECT_FALSE(sut.IsEmpty());

    EXPECT_EQ(1, sut.Pop());
    EXPECT_EQ(0, sut.Size());
    EXPECT_TRUE(sut.IsEmpty());
}
