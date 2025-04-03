#include "Settings.hpp"

#include <core/Engine.hpp>
#include <imgui.h>

namespace Zeus
{
Settings::Settings(UserInterface* root) : m_root{ root }
{
}

void Settings::Update()
{
    if (ImGui::Begin("Settings"))
    {
        if (ImGui::CollapsingHeader("Editor"))
        {
            ImGui::SeparatorText("General");

            if (ImGui::Button("Hide UI"))
            {
                m_root->SetVisible(false);
            }

            ImGui::SameLine();
            ImGui::Text("(F11 to show)");

            ImGui::Checkbox("Statistics", &m_root->options.statistics);
            ImGui::Checkbox("Toolbar", &m_root->options.toolbar);

            ImGui::SeparatorText("Camera");

            const char* items[] = { "Frefly", "Arcball" };
            static int item_current = 0;
            ImGui::Combo("Type", &item_current, items, IM_ARRAYSIZE(items));

            float position[3]{ 1.f, .1f, .4f };
            ImGui::InputFloat3("Position", position);

            float direction[3]{ 1.f, .1f, .4f };
            ImGui::InputFloat3("Direction", direction);
        }

        if (ImGui::CollapsingHeader("Renderer"))
        {
            ImGui::SeparatorText("Swapchain");
            auto& swapchain{ Engine::Renderer().GetSwapchain() };

            ImGui::Text(
                "Image Resolution: %d:%d",
                swapchain.GetWidth(),
                swapchain.GetHeight());

            ImGui::Text("Image Count: %d", swapchain.GetImageCount());
            ImGui::Text("Frame Count: %d", swapchain.GetFramesCount());

            bool vsync{ Engine::Renderer().GetSwapchain().IsVSync() };
            if (ImGui::Checkbox("VSync", &vsync))
            {
                Engine::Renderer().GetSwapchain().SetVsync(vsync);
            }
        }
    }
    ImGui::End();
}
}
