#pragma once

#include <cassert>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <ostream>
#include <queue>
#include <vector>

namespace Zeus
{
template <typename TValue>
int binarySearch(const std::vector<TValue>& values, TValue target)
{
    if (values.empty())
        return false;

    int left{ 0 };
    int right{ static_cast<int>(values.size()) - 1 };
    int mid{ 0 };

    while (left <= right) // will overflow with size_t
    {
        mid = left + (right - left) / 2;
        TValue value = values[static_cast<std::size_t>(mid)];

        if (value == target)
        {
            return mid;
        }
        else if (target > value)
        {
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }

    return -1;
}
}
