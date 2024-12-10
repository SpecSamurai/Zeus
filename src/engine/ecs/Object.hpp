#pragma once

#include "Definitions.hpp"

#include <string_view>

namespace Zeus
{
class Object
{
public:
    Object(std::string_view name = "");

    Id GetId() const noexcept;
    const std::string_view GetName() const noexcept;

private:
    static Id GenerateId() noexcept;

    Id m_id;
    std::string_view m_name;
};
}
