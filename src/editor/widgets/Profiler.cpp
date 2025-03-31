#include "Profiler.hpp"

#include <imgui.h>

namespace Zeus
{
void Profiler::Update()
{
    if (ImGui::Begin("Profiler"))
    {
        ImGui::Text("Frametime %.4f ms", 100.f);
        ImGui::Text("FPS %.4f", 100.f);
        ImGui::Text("Draw time %.4f ms", 100.f);
        ImGui::Text("Update time %.4f ms", 100.f);
        ImGui::Text("Triangles %i", 100);
        ImGui::Text("Draws %i", 100);
    }
    ImGui::End();
}
}
