#pragma once

#include <cstddef>
#include <iostream>
#include <utility>

namespace Zeus
{
// MinHeap
// Complete Binary Tree
// value of each node is <= or >= values of its children
template <typename TValue, int Capacity>
class Heap
{
public:
    // O(log n)
    void Insert(TValue value)
    {
        ++m_size;
        auto i = m_size - 1;

        m_data[i] = value;

        // Bubble up
        while (i > 0 && m_data[i] < m_data[Parent(i)])
        {
            std::swap(m_data[i], m_data[Parent(i)]);
            i = Parent(i);
        }
    }

    // O(log n)
    void Pop()
    {
        std::swap(m_data[0], m_data[m_size - 1]);
        m_data[m_size - 1] = 99999999;
        --m_size;

        Heapify(0);
    }

    // O(1)
    TValue Peek() const
    {
        return m_data[0];
    }

    void Print()
    {
        for (std::size_t i = 0; i < m_size; ++i)
        {
            std::cerr << m_data[i] << " ";
        }
    }

private:
    bool IsLeaf(std::size_t i)
    {
        return i >= m_size / 2;
    }

    void Heapify(std::size_t i)
    {
        std::size_t left{ Left(i) };
        std::size_t right{ Right(i) };
        std::size_t smallest{ i };

        if (left < m_size && m_data[left] < m_data[i])
            smallest = left;

        if (right < m_size && m_data[right] < m_data[smallest])
            smallest = right;

        if (smallest != i)
        {
            std::swap(m_data[i], m_data[smallest]);
            Heapify(smallest);
        }
    }

    static std::size_t Parent(std::size_t i)
    {
        return (i - 1) / 2;
    }

    static std::size_t Left(std::size_t i)
    {
        return 2 * i + 1;
    }

    static std::size_t Right(std::size_t i)
    {
        return 2 * i + 2;
    }

private:
    TValue m_data[Capacity];
    std::size_t m_size{ 0 };
};
}
