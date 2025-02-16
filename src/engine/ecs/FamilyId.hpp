#pragma once

#include <cstdint>

namespace Zeus
{
using Family = std::uint32_t;

class FamilyId
{
public:
    template <typename>
    static Family Type() noexcept
    {
        static const Family id{ Identifier() };
        return id;
    }

private:
    inline static Family Identifier() noexcept
    {
        static Family id{ 0 };
        return id++;
    }
};
}
