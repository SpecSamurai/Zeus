#pragma once

namespace Zeus
{
class Widget
{
public:
    virtual void Update() = 0;

    inline constexpr bool IsVisible() const
    {
        return m_visible;
    }

protected:
    bool m_visible{ true };
};
}
