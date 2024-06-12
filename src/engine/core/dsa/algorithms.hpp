#pragma once

#include "core/dsa/BinarySearchTree.hpp"

#include <cassert>
#include <climits>
#include <cstddef>
#include <iostream>
#include <queue>
#include <set>
#include <stack>
#include <unordered_set>
#include <utility>
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

template <typename TValue>
void treeDfsIterative(BinarySearchTree<TValue>* root, TValue target)
{
    if (root == nullptr || root->m_value == target)
        return root;

    std::stack<BinarySearchTree<TValue>*> stack;

    stack.push(root);

    BinarySearchTree<TValue>* current{ nullptr };
    while (!stack.empty())
    {
        current = stack.top();
        stack.pop();

        // some logic
        if (current->m_value == target)
            return current;

        if (current->m_left != nullptr)
            stack.push(current->m_left);

        if (current->m_right != nullptr)
            stack.push(current->m_right);
    }

    return root;
}

template <typename TValue>
void treeBfs(BinarySearchTree<TValue>* root, TValue target)
{
    if (root == nullptr || root->m_value == target)
        return root;

    std::queue<BinarySearchTree<TValue>*> queue;

    queue.push(root);

    BinarySearchTree<TValue>* current{ nullptr };

    while (!queue.empty())
    {
        auto size = queue.size();
        // logic for current level

        for (std::size_t i = 0; i < size; ++i)
        {
            current = queue.front();
            queue.pop();

            // some logic
            if (current->m_value == target)
                return current;

            if (current->m_left != nullptr)
                queue.push(current->m_left);

            if (current->m_right != nullptr)
                queue.push(current->m_right);
        }
    }

    return root;
}

template <typename TValue>
int graphDfsRecursive(
    std::vector<std::vector<int>>& graph,
    int node,
    std::unordered_set<int> seen)
{
    int ans{ 0 };
    // do some logic
    for (auto neighbor : graph[static_cast<std::size_t>(node)])
    {
        if (seen.find(neighbor) == seen.end())
        {
            seen.insert(neighbor);
            ans += graphDfsRecursive(graph, neighbor, seen);
        }
    }

    return ans;
}

// O(V + E)
inline void dfsSimple(
    std::size_t v,
    std::vector<std::vector<std::size_t>> adj,
    std::vector<bool> visited)
{
    visited[v] = true;
    for (std::size_t u : adj[v])
    {
        if (!visited[u])
            dfsSimple(u, adj, visited);
    }
}

// void dfs(int v) {
// time_in[v] = dfs_timer++;
// Entering the vertex
// color[v] = 1;
// for (int u : adj[v])
//     if (color[u] == 0)
//         dfs(u);
// Exiting the vertex
// color[v] = 2;
// time_out[v] = dfs_timer++;
// }

template <typename TValue>
void graphDfsIterative(std::vector<std::vector<TValue>>& graph)
{
    std::stack<TValue> stack;
    std::unordered_set<int> seen;
    stack.push(0); // START NODE
    seen.insert(0);

    while (!stack.empty())
    {
        auto current = stack.top();
        stack.pop();

        // some logic

        for (TValue neighbor : graph[current])
        {
            if (seen.find(neighbor) == seen.end())
            {
                stack.push(neighbor);
                seen.insert(neighbor);
            }
        }
    }
}

template <typename TValue>
void graphBfs(std::vector<std::vector<TValue>>& graph)
{
    std::queue<TValue> queue;
    std::unordered_set<int> seen;
    queue.push(0); // START NODE
    seen.insert(0);

    while (!queue.empty())
    {
        auto current = queue.front();
        queue.pop();

        // some logic

        for (TValue neighbor : graph[current])
        {
            if (seen.find(neighbor) == seen.end())
            {
                queue.push(neighbor);
                seen.insert(neighbor);
            }
        }
    }
}

inline long long binPowRecusive(long long x, long long n)
{
    if (n == 0)
        return 1;

    if (n < 0)
        return 1 / binPowRecusive(x, -n);

    auto result = binPowRecusive(x, n / 2);

    if (n % 2 == 0)
        return result * result;
    else
        return result * result * x;
}

inline long long binPowIterative(long long x, long long n)
{
    long long result = 1;

    while (n > 0)
    {
        if (n & 1)
            result *= x;

        x *= x;
        n >>= 1;
    }

    return result;
}

// O(n)
// It is commonly used to find the next greater or smaller element in an array
inline void monotonicStack(const std::vector<int>& array)
{
    std::stack<int> stack;

    for (int value : array)
    {
        while (!stack.empty() && value < stack.top())
        {
            stack.pop();
        }

        stack.push(value);
    }
}

inline std::size_t minDistance(std::vector<int> dist, std::vector<bool> visited)
{
    std::size_t minIndex{ 0 };
    int min{ INT_MAX };

    for (std::size_t i{ 0 }; i < dist.size(); ++i)
    {
        if (!visited[i] && dist[i] <= min)
        {
            min = dist[i];
            minIndex = i;
        }
    }

    return minIndex;
}

inline void dijkstraQueue(
    std::vector<std::vector<std::pair<std::size_t, int>>> adj,
    std::size_t start)
{
    std::size_t size = adj.size();
    // lengths of shortest paths from start to all vertices
    std::vector<int> dist(size, INT_MAX);
    // penultimate vertex in the shortest path from start to each vertex
    std::vector<int> prev(size, -1);
    std::set<std::pair<int, std::size_t>> queue;

    dist[start] = 0;
    queue.insert({ 0, start });

    while (!queue.empty())
    {
        std::size_t current{ queue.begin()->second };
        queue.erase(queue.begin());

        // if use heap with duplicates we check if it is an important pair or if
        // it is already an old and handled pair otherwise the complexity can
        // increase up to O(nm)
        // if (d_v != d[v])
        //    continue;

        for (auto edge : adj[current])
        {
            auto to = edge.first;
            auto weight = edge.second;

            auto newPath = dist[current] + weight;
            if (newPath < dist[to])
            {
                queue.erase({ dist[to], to });
                dist[to] = newPath;
                prev[to] = static_cast<int>(current);
                queue.insert({ dist[to], to });
            }
        }
    }

    std::cerr << "End";
}

inline void dijkstra(
    std::vector<std::vector<std::pair<std::size_t, int>>> adj,
    std::size_t start)
{
    std::size_t size = adj.size();
    // lengths of shortest paths from start to all vertices
    std::vector<int> dist(size, INT_MAX);
    // penultimate vertex in the shortest path from start to each vertex
    std::vector<int> prev(size, -1);
    std::vector<bool> visited(size, false);

    // dist[start] = 0;

    for (std::size_t i{ 0 }; i < size; ++i)
    {
        std::size_t current{ minDistance(dist, visited) };

        // vertices are unreachable from the starting vertex
        // algorithm can be stopped as soon as the selected vertex has infinite
        // distance to it
        if (dist[current] == INT_MAX)
            break;

        visited[current] = true;

        for (auto edge : adj[current])
        {
            auto to = edge.first;
            auto weight = edge.second;

            auto newPath = dist[current] + weight;
            if (newPath < dist[to])
            {
                dist[to] = newPath;
                prev[to] = static_cast<int>(current);
            }
        }
    }

    std::cerr << "End";
}

void aStar();

inline void dfs(
    std::size_t vertex,
    std::vector<std::vector<std::size_t>>& adj,
    std::vector<bool>& visited,
    std::vector<std::size_t>& ans)
{
    visited[vertex] = true;

    for (auto u : adj[vertex])
    {
        if (!visited[u])
            dfs(u, adj, visited, ans);
    }

    ans.push_back(vertex);
}

inline void topologicalSort(std::vector<std::vector<std::size_t>> adj)
{
    std::vector<bool> visited(adj.size(), false);
    std::vector<std::size_t> ans(adj.size());

    for (std::size_t i{ 0 }; i < adj.size(); ++i)
    {
        if (!visited[i])
        {
            dfs(i, adj, visited, ans);
        }
    }
}

// Kahn’s Algorithm
inline void topologicalSortBfs(std::vector<std::vector<std::size_t>> adj)
{
    std::vector<int> inDegree(adj.size(), 0);

    for (std::size_t i{ 0 }; i < adj.size(); ++i)
    {
        for (auto const& w : adj[i])
            ++inDegree[w];
    }

    std::queue<std::size_t> queue;
    for (std::size_t i{ 0 }; i < adj.size(); ++i)
    {
        if (inDegree[i] == 0)
            queue.push(i);
    }

    std::vector<std::size_t> topOrder;
    std::size_t visitedVertices{ 0 };

    while (!queue.empty())
    {
        auto current = queue.front();
        queue.pop();
        topOrder.push_back(current);

        for (auto n : adj[current])
        {
            if (--inDegree[n] == 0)
            {
                queue.push(n);
            }
        }

        ++visitedVertices;
    }

    if (visitedVertices != adj.size())
    {
        std::cerr << "Graph contains cycle\n";
        return;
    }

    for (auto i : topOrder)
        std::cerr << i << " ";
}

inline int partition2(int arr[], int left, int right)
{
    int pivot = arr[right];
    int i = left - 1;

    for (int j{ left }; j < right; ++j)
    {
        if (arr[j] < pivot)
        {
            ++i;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[right]);
    return i + 1;
}

inline void quickSort2(int arr[], int low, int high)
{
    if (low < high)
    {
        int pivotIndex = partition2(arr, low, high);
        quickSort2(arr, low, pivotIndex - 1);
        quickSort2(arr, pivotIndex + 1, high);
    }
}

inline std::size_t partition(
    std::vector<int>& array,
    std::size_t left,
    std::size_t right,
    std::size_t pivotIndex)
{
    auto pivot = array[pivotIndex];

    while (left <= right)
    {
        while (array[left] < pivot)
        {
            ++left;
        }

        while (array[right] > pivot)
        {
            --right;
        }

        if (left <= right)
        {
            std::swap(array[left], array[right]);
            ++left;
            --right;
        }
    }

    return left;
}

inline void quickSort(
    std::vector<int>& array,
    std::size_t left,
    std::size_t right)
{
    if (left >= right)
        return;

    auto pivot = left + (right - left) / 2;
    auto index = partition(array, left, right, pivot);
    quickSort(array, left, index - 1);
    quickSort(array, index, right);
}

inline void quickSort(std::vector<int>& array)
{
    quickSort(array, 0, array.size() - 1);
}

// First subarray is arr[begin..mid]
// Second subarray is arr[mid+1..end]
inline void merge(
    std::vector<int>& array,
    const std::vector<int>& leftHalf,
    const std::vector<int>& rightHalf)
{
    std::size_t leftIndex{ 0 };
    std::size_t rightIndex{ 0 };
    std::size_t mergedIndex{ 0 };

    while (leftIndex < leftHalf.size() && rightIndex < rightHalf.size())
    {
        if (leftHalf[leftIndex] <= rightHalf[rightIndex])
        {
            array[mergedIndex] = leftHalf[leftIndex];
            ++leftIndex;
        }
        else
        {
            array[mergedIndex] = rightHalf[rightIndex];
            ++rightIndex;
        }

        ++mergedIndex;
    }

    // copy remaining elements in leftHalf
    while (leftIndex < leftHalf.size())
    {
        array[mergedIndex] = leftHalf[leftIndex];
        ++mergedIndex;
        ++leftIndex;
    }

    // copy remaining elements in rightHalf
    while (rightIndex < rightHalf.size())
    {
        array[mergedIndex] = rightHalf[rightIndex];
        ++mergedIndex;
        ++rightIndex;
    }
}

// O(n*log n)
// Split
inline void mergeSort(std::vector<int>& array)
{
    auto size = array.size();
    // when there is no or one item
    if (size < 2)
        return; // return the item

    auto mid = size / 2;
    std::vector<int> leftHalf(mid);
    std::vector<int> rightHalf(size - mid);

    for (std::size_t i{ 0 }; i < mid; ++i)
    {
        leftHalf[i] = array[i];
    }

    for (std::size_t i{ mid }; i < size; ++i)
    {
        rightHalf[i - mid] = array[i];
    }

    mergeSort(leftHalf);
    mergeSort(rightHalf);
    merge(array, leftHalf, rightHalf);
}

class ListNode
{
public:
    int val;
    ListNode* next;
    ListNode(int x)
    {
        val = x;
        next = nullptr;
    }
};

// Fast and slow pointers
// - Find center: travel 2*slow = center when fast == null
// - Array intersection by rerouting end of array to the beginning
inline bool listDetectCycle(ListNode* head)
{
    ListNode* slow{ head };
    ListNode* fast{ head };

    while (fast != nullptr && fast->next != nullptr)
    {
        slow = slow->next;
        fast = fast->next->next;

        if (slow == fast)
            return true;
    }

    return false;
}

inline bool cycleLength(ListNode* head)
{
    ListNode* slow{ head };
    ListNode* fast{ head };
    std::size_t length{ 0 };

    while (fast != nullptr && fast->next != nullptr)
    {
        slow = slow->next;
        fast = fast->next->next;

        if (slow == fast)
        {
            while (true)
            {
                slow = slow->next;
                ++length;

                if (slow == fast)
                    return length;
            }
        }
    }

    return length;
}

// slow = a + b
// fast = a + b + c + b = 2(a+b)
// a = c
inline ListNode* findCycleNode(ListNode* head)
{
    ListNode* slow{ head };
    ListNode* fast{ head };

    while (fast != nullptr && fast->next != nullptr)
    {
        slow = slow->next;
        fast = fast->next->next;

        if (slow == fast)
        {
            break;
        }
    }

    slow = head;
    while (slow != fast)
    {
        slow = slow->next;
        fast = fast->next;
    }

    // remove node
    while (slow->next != fast)
        slow = slow->next;

    slow->next = nullptr;

    return slow;
}

bool isCyclicUtil(
    std::size_t v,
    std::vector<std::vector<std::size_t>>& adj,
    bool visited[],
    bool* recStack)
{
    if (visited[v] == false)
    {
        visited[v] = true;
        recStack[v] = true;

        for (auto i : adj[v])
        {
            if (!visited[i] && isCyclicUtil(i, adj, visited, recStack))
                return true;
            else if (recStack[i])
                return true;
        }
    }

    recStack[v] = false;
    return false;
}

inline bool isCyclic(std::vector<std::vector<std::size_t>> adj)
{
    bool* visited = new bool[adj.size()];
    bool* recStack = new bool[adj.size()];
    for (std::size_t i = 0; i < adj.size(); i++)
    {
        visited[i] = false;
        recStack[i] = false;
    }

    for (std::size_t i = 0; i < adj.size(); i++)
        if (!visited[i] && isCyclicUtil(i, adj, visited, recStack))
            return true;

    return false;
}

inline bool isCyclicKhan(std::vector<std::vector<std::size_t>> adj)
{
    std::vector<int> inDegree(adj.size(), 0);
    std::queue<std::size_t> q; // queue to store vertices with 0 in-degree
    std::size_t visited = 0;

    // calculate in-degree of each vertex
    for (std::size_t u = 0; u < adj.size(); u++)
    {
        for (auto v : adj[u])
        {
            inDegree[v]++;
        }
    }

    // enqueue vertices with 0 in-degree
    for (std::size_t u = 0; u < adj.size(); u++)
    {
        if (inDegree[u] == 0)
        {
            q.push(u);
        }
    }

    // BFS traversal
    while (!q.empty())
    {
        auto u = q.front();
        q.pop();
        visited++;

        // reduce in-degree of adjacent vertices
        for (auto v : adj[u])
        {
            inDegree[v]--;
            // if in-degree becomes 0, enqueue the
            // vertex
            if (inDegree[v] == 0)
            {
                q.push(v);
            }
        }
    }

    return visited != adj.size(); // if not all vertices are
                                  // visited, there is a cycle
}
}
