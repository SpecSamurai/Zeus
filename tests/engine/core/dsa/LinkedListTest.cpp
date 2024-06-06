#include <core/dsa/LinkedList.hpp>

#include "gtest/gtest.h"
#include <cstddef>

TEST(LinkedListTest, Get_IndexEqualSize_LastElement)
{
    auto sut = Zeus::LinkedList<int>();

    sut.Append(1);
    EXPECT_EQ(1, sut.Get(0));

    sut.Append(2);
    EXPECT_EQ(2, sut.Get(1));

    EXPECT_EQ(2, sut.Size());
    EXPECT_FALSE(sut.IsEmpty());
}

TEST(LinkedListTest, Get_Index_MultipleElements)
{
    auto sut = Zeus::LinkedList<int>();
    sut.Append(1);
    sut.Append(2);
    sut.Append(3);

    EXPECT_EQ(1, sut.Get(0));
    EXPECT_EQ(2, sut.Get(1));
    EXPECT_EQ(3, sut.Get(2));

    EXPECT_EQ(3, sut.Size());
    EXPECT_FALSE(sut.IsEmpty());
}

TEST(LinkedListTest, Contains)
{
    auto sut = Zeus::LinkedList<int>();

    EXPECT_FALSE(sut.Contains(1));

    sut.Append(1);
    sut.Append(2);
    sut.Append(3);

    EXPECT_TRUE(sut.Contains(1));
    EXPECT_TRUE(sut.Contains(2));
    EXPECT_TRUE(sut.Contains(3));
    EXPECT_FALSE(sut.Contains(4));
}

TEST(LinkedListTest, Add_IndexOutOfBounds_False)
{
    auto sut = Zeus::LinkedList<int>();

    EXPECT_FALSE(sut.Add(1, 1));
    EXPECT_TRUE(sut.Add(1, 0));
    EXPECT_FALSE(sut.Add(1, 2));
}

TEST(LinkedListTest, Add_AtTheEdgeOfTheBoundary_True)
{
    auto sut = Zeus::LinkedList<int>();

    EXPECT_TRUE(sut.Add(1, 0)); // empty
    EXPECT_TRUE(sut.Add(2, 0)); // head index
    EXPECT_TRUE(sut.Add(3, 1)); // tail

    EXPECT_EQ(2, sut.Get(0));
    EXPECT_EQ(3, sut.Get(1));
    EXPECT_EQ(1, sut.Get(2));
}

TEST(LinkedListTest, Add_MiltipleElements)
{
    auto sut = Zeus::LinkedList<int>();

    EXPECT_TRUE(sut.Add(1, 0));
    EXPECT_TRUE(sut.Add(2, 1));
    EXPECT_TRUE(sut.Add(3, 2));
    EXPECT_TRUE(sut.Add(4, 2));
    EXPECT_TRUE(sut.Add(5, 1));

    EXPECT_EQ(1, sut.Get(0));
    EXPECT_EQ(5, sut.Get(1));
    EXPECT_EQ(2, sut.Get(2));
    EXPECT_EQ(4, sut.Get(3));
    EXPECT_EQ(3, sut.Get(4));

    EXPECT_EQ(5, sut.Size());
}

TEST(LinkedListTest, Delete_IndexOutOfBounds_False)
{
    auto sut = Zeus::LinkedList<int>();

    EXPECT_FALSE(sut.Delete(0));
    sut.Append(1);
    EXPECT_FALSE(sut.Delete(1));

    EXPECT_TRUE(sut.Delete(0));
    EXPECT_FALSE(sut.Delete(0));
}

TEST(LinkedListTest, Delete_MultipleElements)
{
    auto sut = Zeus::LinkedList<int>();
    sut.Append(1);
    sut.Append(2);
    sut.Append(3);
    sut.Append(4);
    sut.Append(5);

    sut.Delete(0);
    EXPECT_EQ(2, sut.Get(0));
    EXPECT_EQ(3, sut.Get(1));
    EXPECT_EQ(4, sut.Get(2));
    EXPECT_EQ(5, sut.Get(3));

    sut.Delete(3);
    EXPECT_EQ(2, sut.Get(0));
    EXPECT_EQ(3, sut.Get(1));
    EXPECT_EQ(4, sut.Get(2));

    sut.Delete(1);
    EXPECT_EQ(2, sut.Get(0));
    EXPECT_EQ(4, sut.Get(1));

    EXPECT_EQ(2, sut.Size());
}
