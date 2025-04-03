#pragma once

#include "UserInterface.hpp"
#include "Widget.hpp"

namespace Zeus
{
class Settings : public Widget
{
public:
    Settings(UserInterface* root);

    void Update() override;

private:
    UserInterface* m_root;
};
}
