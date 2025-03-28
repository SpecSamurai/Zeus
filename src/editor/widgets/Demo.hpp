#pragma once

#include "widgets/Widget.hpp"

#include <imgui.h>

namespace Zeus
{
class Demo : public Widget
{
public:
    void Update() override
    {
        ImGui::ShowDemoWindow();
    }
};
}
