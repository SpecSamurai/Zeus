#pragma once

#include <cstddef>

namespace Zeus
{
template <typename TValue>
class LinkedList
{
private:
    class Node
    {
    public:
        Node(TValue value, Node* prev = nullptr, Node* next = nullptr)
            : m_prev{ prev },
              m_next{ next },
              m_value{ value }
        {
        }

    private:
        Node* m_prev;
        Node* m_next;
        TValue m_value;

        friend class LinkedList;
    };

public:
    LinkedList() = default;

    ~LinkedList()
    {
        Node* current{ nullptr };

        while (m_head != nullptr)
        {
            current = m_head->m_next;

            delete m_head;

            m_head = current;
        }

        m_tail = nullptr;
    }

    bool Add(TValue value, std::size_t index)
    {
        if (index > m_size)
            return false;

        Node* node{ new Node(value) };

        if (IsEmpty())
        {
            m_head = m_tail = node;
        }
        else if (index == 0)
        {
            node->m_next = m_head;
            m_head->m_prev = node;
            m_head = node;
        }
        else if (index == m_size)
        {
            m_tail->m_next = node;
            node->m_prev = m_tail;
            m_tail = node;
        }
        else
        {
            Node* current{ m_head };
            while (index--)
                current = current->m_next;

            node->m_next = current;
            node->m_prev = current->m_prev;

            current->m_prev->m_next = node;
            current->m_prev = node;
        }

        ++m_size;

        return true;
    }

    bool Delete(std::size_t index)
    {
        if (index >= m_size)
            return false;

        if (m_size == 1)
        {
            delete m_head;

            m_head = nullptr;
            m_tail = nullptr;
        }
        else if (index == 0)
        {
            m_head = m_head->m_next;
            delete m_head->m_prev;
            m_head->m_prev = nullptr;
        }
        else if (index == m_size - 1)
        {
            m_tail = m_tail->m_prev;
            delete m_tail->m_next;
            m_tail->m_next = nullptr;
        }
        else
        {
            Node* current{ m_head };

            while (index--)
                current = current->m_next;

            current->m_prev->m_next = current->m_next;
            current->m_next->m_prev = current->m_prev;

            delete current;
            current = nullptr;
        }

        --m_size;
        return true;
    }

    bool Contains(TValue value) const
    {
        Node* current{ m_head };

        while (current != nullptr)
        {
            if (current->m_value == value)
                return true;

            current = current->m_next;
        }

        return false;
    }

    TValue Get(std::size_t index) const
    {
        if (index == m_size - 1)
            return m_tail->m_value;

        Node* current{ m_head };
        while (index--)
            current = current->m_next;

        return current->m_value;
    }

    bool Append(TValue value)
    {
        return Add(value, m_size);
    }

    TValue GetHead() const
    {
        return m_head->m_value;
    }

    TValue GetTail() const
    {
        return m_tail->m_value;
    }

    bool IsEmpty() const
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
