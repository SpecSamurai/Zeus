#pragma once

#include "ICommand.hpp"

#include <array>
#include <cstdint>
#include <memory>
#include <utility>

namespace Zeus
{
template <std::uint64_t Size>
class CommandStack
{
public:
    template <typename CommandType, typename... Args>
    void Push(Args&&... args)
    {
        if (m_undone != 0)
        {
            m_count -= m_undone;
            m_undone = 0;
        }

        m_data[m_top] =
            std::make_shared<CommandType>(std::forward<Args>(args)...);

        m_top = (m_top + 1) % Size;

        if (m_count != Size)
            ++m_count;
    }

    void Undo()
    {
        if (m_undone == m_count)
            return;

        m_top = (m_top - 1 + Size) % Size;

        m_data[m_top]->Undo();

        ++m_undone;
    }

    void Redo()
    {
        if (m_undone == 0)
            return;

        m_data[m_top]->Execute();

        m_top = (m_top + 1) % Size;
        --m_undone;
    }

    std::uint64_t Count()
    {
        return m_count;
    }

    std::uint64_t Undone()
    {
        return m_undone;
    }

    bool Empty()
    {
        return m_count == 0;
    }

    void Clear()
    {
        m_top = 0;
        m_count = 0;
        m_undone = 0;
    }

private:
    std::array<std::shared_ptr<ICommand>, Size> m_data;

    std::uint64_t m_top{ 0 };
    std::uint64_t m_count{ 0 };
    std::uint64_t m_undone{ 0 };
};
}
