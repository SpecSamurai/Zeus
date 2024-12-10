#pragma once

#include "Object.hpp"

#include "Definitions.hpp"

#include <string_view>

namespace Zeus
{
Object::Object(std::string_view name) : m_id{ GenerateId() }, m_name{ name }
{
}

Id Object::GetId() const noexcept
{
    return m_id;
}

const std::string_view Object::GetName() const noexcept
{
    return m_name;
}

Id Object::GenerateId() noexcept
{
    static Id id{ 0 };
    return id++;
}
}
