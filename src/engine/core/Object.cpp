#include "Object.hpp"

#include <string_view>

namespace Zeus
{
Object::Object(std::string_view name) : m_name{ name }
{
}

constexpr std::string_view Object::GetName() const noexcept
{
    return m_name;
}
}
