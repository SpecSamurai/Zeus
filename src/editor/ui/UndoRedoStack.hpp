#pragma once

#include <iostream>
#include <memory>  // for std::unique_ptr
#include <utility> // for std::move

#include <cstddef>
#include <cstdint>

using namespace std;

namespace Zeus
{
template <int32_t Capacity>
class UndoRedoStack
{
public:
    void push(int value)
    {
        // std::nullptr_t a{};

        if (m_undone != 0)
        {
            m_size -= m_undone;
            m_undone = 0;
        }

        m_commands[m_top] = value;
        cout << "Pushed: " << m_commands[m_top] << endl;

        m_top = (m_top + 1) % Capacity;

        if (m_size != Capacity)
            ++m_size;
    }

    /*[[nodiscard]]*/ void pop()
    {
        if (m_size == 0)
        {
            cout << "Empty! " << endl;
            return;
        }

        m_top = (m_top - 1 + Capacity) % Capacity;
        --m_size;
        cout << "Poped: " << m_commands[m_top] << endl;
    }

    void undo()
    {
        if (m_undone == m_size)
        {
            cout << "Nothing to undo! " << endl;
            return;
        }

        m_top = (m_top - 1 + Capacity) % Capacity;
        ++m_undone;
        cout << "Undone: " << m_commands[m_top] << endl;
    }

    void redo()
    {
        if (m_undone == 0)
        {
            cout << "Nothing to redo! " << endl;
            return;
        }

        cout << "Redone: " << m_commands[m_top] << endl;
        m_top = (m_top + 1) % Capacity;
        --m_undone;
    }

    void clear()
    {
        // for (int32_t i = 0; i < m_commands; ++i)
        // {
        //     delete m_commands[i];
        //     m_commands[i] = nullptr;
        // }
    }

    constexpr const int32_t* getCommands()
    {
        return m_commands;
    }

private:
    int32_t m_commands[Capacity]{};
    int32_t m_top{ 0 };
    int32_t m_size{ 0 };
    int32_t m_undone{ 0 };
};
} // namespace Zeus
