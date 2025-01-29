#pragma once

namespace Zeus
{
class FamilyId
{
public:
    template <typename>
    static int Type() noexcept
    {
        static const int id{ Identifier() };
        return id;
    }

private:
    inline static int Identifier() noexcept
    {
        static int id{ 0 };
        return id++;
    }
};
}
