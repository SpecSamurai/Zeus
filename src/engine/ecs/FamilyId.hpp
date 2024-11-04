#pragma once

#include "Definitions.hpp"

namespace Zeus
{
class FamilyId
{
public:
    template <typename>
    static Id Type() noexcept
    {
        static const Id id{ Identifier() };
        return id;
    }

private:
    inline static Id Identifier() noexcept
    {
        static Id id{ 0 };
        return id++;
    }
};
}
