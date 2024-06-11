#pragma once

#include <cstddef>
#include <utility>

namespace Zeus
{
// final amortized time complexity is O(a(n)) where a(n) is the inverse
// Ackermann function
// in our case a single call might take O(log n) in the worst case
template <std::size_t Capacity>
class DisjointSet
{
public:
    DisjointSet()
    {
        for (std::size_t i{ 0 }; i < Capacity; ++i)
        {
            parent[i] = static_cast<int>(i);
        }
    }

    void MakeSet(int value)
    {
        parent[value] = value;
        rank[value] = 1; // depth = 0
    }

    void UnionSets(int first, int second)
    {
        first = FindSet(first);
        second = FindSet(second);

        if (first != second)
        {
            // Union by rank
            if (rank[first] < rank[second])
                std::swap(first, second);

            parent[second] = first;
            rank[first] += rank[second]; // depth ++rank[first];
        }
    }

    int FindSet(int value)
    {
        if (value == parent[value])
            return value;
        else
            // Path compression
            return parent[value] = FindSet(parent[value]);
    }

private:
    int parent[Capacity];
    int rank[Capacity];
};
}
