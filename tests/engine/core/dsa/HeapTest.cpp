#include <core/dsa/Heap.hpp>

#include "gtest/gtest.h"

TEST(HeapTest, Insert)
{
    auto sut{ Zeus::Heap<int, 10>() };

    sut.Insert(5);
    EXPECT_EQ(5, sut.Peek());

    sut.Insert(4);
    EXPECT_EQ(4, sut.Peek());

    sut.Insert(2);
    EXPECT_EQ(2, sut.Peek());

    sut.Insert(1);
    EXPECT_EQ(1, sut.Peek());

    sut.Insert(3);
    EXPECT_EQ(1, sut.Peek());

    sut.Print();

    sut.Pop();
    EXPECT_EQ(2, sut.Peek());

    sut.Pop();
    EXPECT_EQ(3, sut.Peek());

    sut.Pop();
    EXPECT_EQ(4, sut.Peek());

    sut.Pop();
    EXPECT_EQ(5, sut.Peek());
}
