#pragma once

#include <string_view>

namespace Zeus
{
class Object
{
public:
    Object(std::string_view name = "");

    constexpr std::string_view GetName() const noexcept;

protected:
    std::string_view m_name{};
};
}
