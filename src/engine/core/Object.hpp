#pragma once

#include <string_view>

namespace Zeus
{
class Object
{
public:
    constexpr Object(std::string_view name = "") : m_name{ name }
    {
    }

    constexpr std::string_view GetName() const noexcept
    {
        return m_name;
    }

protected:
    std::string_view m_name{};
};
}
