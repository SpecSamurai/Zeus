#pragma once

#include <cstddef>
namespace Zeus
{
template <typename TValue>
class Stack
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

        friend class Stack;
    };

public:
    Stack() = default;

    ~Stack()
    {
        Node* temp{ nullptr };

        while (m_top != nullptr)
        {
            temp = m_top->m_next;
            delete m_top;
            m_top = temp;
        }
    }

    void Push(TValue value)
    {
        Node* node{ new Node(value, m_top) };

        m_top = node;
        ++m_size;
    }

    [[nodiscard]] TValue Pop()
    {
        TValue value{ m_top->m_value };
        Node* next{ m_top->m_next };

        delete m_top;
        m_top = next;

        --m_size;
        return value;
    }

    TValue Peek()
    {
        return m_top->m_value;
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
    Node* m_top{ nullptr };
    std::size_t m_size{ 0 };
};
}
