#include <core/dsa/algorithms.hpp>

#include "gtest/gtest.h"
#include <iostream>
#include <utility>

TEST(AlgorithmsTest, binarySearch_collection)
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

TEST(AlgorithmsTest, mergeSort)
{
    std::vector<int> values{ 4, 2, 1, 4, -3, 11, -5, 3 };

    Zeus::mergeSort(values);

    for (auto value : values)
    {
        std::cerr << value << " ";
    }
}

TEST(AlgorithmsTest, quickSort)
{
    std::vector<int> values{ 4, 2, 1, 4, -3, 11, -5, 3 };

    Zeus::quickSort(values);

    for (auto value : values)
    {
        std::cerr << value << " ";
    }
}

TEST(AlgorithmsTest, quickSort2)
{
    int values[8]{ 4, 2, 1, 4, -3, 11, -5, 3 };

    Zeus::quickSort2(values, 0, 7);

    for (auto value : values)
    {
        std::cerr << value << " ";
    }
}

void addEdge(
    std::vector<std::vector<std::pair<std::size_t, int>>>& adj,
    std::size_t u,
    std::size_t v,
    std::size_t w)
{
    adj[u].push_back(std::make_pair(v, w));
    adj[v].push_back(std::make_pair(u, w));
}

void addEdge(
    std::vector<std::vector<std::size_t>>& adj,
    std::size_t u,
    std::size_t v)
{
    adj[u].push_back(v);
}

TEST(AlgorithmsTest, dijkstra)
{
    std::vector<std::vector<std::pair<std::size_t, int>>> adj(9);

    addEdge(adj, 0, 1, 4);
    addEdge(adj, 0, 7, 8);
    addEdge(adj, 1, 2, 8);
    addEdge(adj, 1, 7, 11);
    addEdge(adj, 2, 3, 7);
    addEdge(adj, 2, 8, 2);
    addEdge(adj, 2, 5, 4);
    addEdge(adj, 3, 4, 9);
    addEdge(adj, 3, 5, 14);
    addEdge(adj, 4, 5, 10);
    addEdge(adj, 5, 6, 2);
    addEdge(adj, 6, 7, 1);
    addEdge(adj, 6, 8, 6);
    addEdge(adj, 7, 8, 7);

    Zeus::dijkstraQueue(adj, 0);
}

TEST(AlgorithmsTest, topologicalSort)
{
    std::vector<std::vector<std::size_t>> adj(6);
    addEdge(adj, 5, 2);
    addEdge(adj, 5, 0);
    addEdge(adj, 4, 0);
    addEdge(adj, 4, 1);
    addEdge(adj, 2, 3);
    addEdge(adj, 3, 1);

    Zeus::topologicalSortBfs(adj);
}
