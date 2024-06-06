#pragma once

#include <cstddef>

namespace Zeus
{
template <typename TValue>
class Queue
{
private:
    class Node
    {
    public:
        Node(TValue value, Node* next = nullptr)
            : m_next{ next },
              m_value{ value }
        {
        }

    private:
        Node* m_next;
        TValue m_value;

        friend class Queue;
    };

public:
    Queue() = default;

    ~Queue()
    {
        Node* temp{ nullptr };

        while (m_head != nullptr)
        {
            temp = m_head->m_next;
            delete m_head;
            m_head = temp;
        }
    }

    void Enqueue(TValue value)
    {
        Node* node{ new Node(value) };

        if (IsEmpty())
        {
            m_head = m_tail = node;
        }
        else
        {
            m_tail->m_next = node;
            m_tail = node;
        }

        ++m_size;
    }

    [[nodiscard]] TValue Dequeue()
    {
        TValue value{ m_head->m_value };
        Node* next{ m_head->m_next };

        delete m_head;
        m_head = next;

        --m_size;
        return value;
    }

    TValue Peek()
    {
        return m_head->m_value;
    }

    bool IsEmpty()
    {
        return m_size == 0;
    }

    std::size_t Size()
    {
        return m_size;
    }

private:
    Node* m_head{ nullptr };
    Node* m_tail{ nullptr };
    std::size_t m_size{ 0 };
};
}
