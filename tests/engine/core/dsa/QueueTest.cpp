#include <core/dsa/Queue.hpp>

#include "gtest/gtest.h"

TEST(QueueTest, Enqueue)
{
    auto sut = Zeus::Queue<int>();

    sut.Enqueue(1);
    EXPECT_EQ(1, sut.Peek());
    EXPECT_EQ(1, sut.Size());

    sut.Enqueue(2);
    EXPECT_EQ(1, sut.Peek());
    EXPECT_EQ(2, sut.Size());
}

TEST(QueueTest, Dequeue)
{
    auto sut = Zeus::Queue<int>();

    sut.Enqueue(1);
    sut.Enqueue(2);
    sut.Enqueue(3);

    EXPECT_EQ(1, sut.Dequeue());
    EXPECT_EQ(2, sut.Dequeue());
    EXPECT_EQ(3, sut.Dequeue());

    EXPECT_EQ(0, sut.Size());
    EXPECT_TRUE(sut.IsEmpty());
}
