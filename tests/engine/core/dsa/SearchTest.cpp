#include <core/dsa/search.hpp>

#include "gtest/gtest.h"

TEST(SearchTest, binarySearch_collection)
{
    std::vector<int> values{ 1, 2, 3, 4, 5 };

    EXPECT_EQ(0, Zeus::binarySearch(values, 1));
    EXPECT_EQ(1, Zeus::binarySearch(values, 2));
    EXPECT_EQ(2, Zeus::binarySearch(values, 3));
    EXPECT_EQ(3, Zeus::binarySearch(values, 4));
    EXPECT_EQ(4, Zeus::binarySearch(values, 5));

    EXPECT_EQ(-1, Zeus::binarySearch(values, 0));
    EXPECT_EQ(-1, Zeus::binarySearch(values, 6));
}
