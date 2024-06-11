#include <core/dsa/DisjointSet.hpp>

#include "gtest/gtest.h"

TEST(DisjointSetTest, UnionFind)
{
    Zeus::DisjointSet<10> sut;

    sut.MakeSet(1);
    sut.MakeSet(2);

    EXPECT_EQ(1, sut.FindSet(1));
    EXPECT_EQ(2, sut.FindSet(2));

    sut.UnionSets(1, 2);

    EXPECT_EQ(sut.FindSet(1), sut.FindSet(2));
}
