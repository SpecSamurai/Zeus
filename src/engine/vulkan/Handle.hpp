#pragma once

#include <cstdint>

namespace Zeus
{
class Handle
{
protected:
    Handle(const char* name) : m_name{ name }, m_id{ 0 }
    {
    }

    Handle(std::uint64_t id, const char* name) : m_name{ name }, m_id{ id }
    {
    }

public:
    constexpr std::uint64_t GetId() const
    {
        return m_id;
    }

    const char* GetName() const
    {
        return m_name;
    }

protected:
    const char* m_name;
    const std::uint64_t m_id;
};
}
