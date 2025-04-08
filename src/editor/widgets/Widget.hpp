#pragma once

namespace Zeus
{
class Widget
{
public:
    inline virtual void Initialize()
    {
    }

    inline virtual void Destroy()
    {
    }

    virtual void Update() = 0;

    inline constexpr void ToggleVisible()
    {
        m_visible ^= 1;
    }

    inline constexpr bool IsVisible() const
    {
        return m_visible;
    }

protected:
    bool m_visible{ true };
};
}
