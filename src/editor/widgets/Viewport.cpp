#include "Viewport.hpp"

#include "UserInterface.hpp"
#include "events/Event.hpp"
#include "input/KeyCode.hpp"

#include <backends/imgui_impl_vulkan.h>
#include <core/Engine.hpp>
#include <imgui.h>
#include <profiling/Profiler.hpp>

namespace Zeus
{
Viewport::Viewport(UserInterface* root) : m_root{ root }
{
}

void Viewport::Initialize()
{
    m_image =
        &Engine::Renderer().GetRenderTarget(RenderTarget::RENDER_OUTPUT_COLOR);

    m_descriptorSet = ImGui_ImplVulkan_AddTexture(
        Engine::Renderer()
            .GetSampler(SamplerType::LINEAR_CLAMP_EDGE)
            .GetHandle(),
        m_image->GetView(),
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void Viewport::Destroy()
{
    ImGui_ImplVulkan_RemoveTexture(m_descriptorSet);
}

void Viewport::Update()
{
    ImGuiWindowFlags windowFlags{ ImGuiWindowFlags_NoScrollbar };

    bool open{ true };
    if (ImGui::Begin("Viewport", &open, windowFlags))
    {
        float width{ static_cast<float>(m_image->GetWidth()) };
        float height{ static_cast<float>(m_image->GetHeight()) };

        ImGui::Image(
            reinterpret_cast<ImTextureID>(m_descriptorSet),
            ImVec2(width, height),
            ImVec2(0, 0),
            ImVec2(1, 1),
            ImVec4(1, 1, 1, 1),
            ImColor(0, 0, 0, 0));

        ShowToolbar(m_root->options.toolbar);
        ShowStatistics(m_root->options.statistics);
    }
    ImGui::End();
}

void Viewport::ShowToolbar(bool show)
{
    if (show)
    {
        ImVec2 parentSize{ ImGui::GetWindowSize() };

        ImGui::SetCursorPos(ImVec2(parentSize.x - 200, 30));
        ImGui::SetNextWindowBgAlpha(.9f);

        if (ImGui::BeginChild("Statistics", ImVec2(175, 102), true))
        {
            ImGui::Text("Frametime: %.4f ms", Profiler::LastFrametime());
            ImGui::Text("FPS: %.1f", Profiler::FPS());
            ImGui::Text("Draw time: %.4f ms", Profiler::DrawTime());
            ImGui::Text("Update time: %.4f ms", Profiler::UpdateTime());
            ImGui::Text("Triangles: %i", Profiler::TriangleCount());
            ImGui::Text("Draws: %i", Profiler::DrawCallCount());
        }
        ImGui::EndChild();
    }
}

void Viewport::ShowStatistics(bool show)
{
    if (show)
    {
        ImGui::SetCursorPos(ImVec2(10, 30));
        ImGui::SetNextWindowBgAlpha(.9f);

        if (ImGui::BeginChild("Toolbar", ImVec2(19, 23), true))
        {
            if (ImGui::Button("X"))
            {
                Zeus::Event::Dispatcher.Dispatch(
                    KeyPressedEvent{ .keyCode = KeyCode::C });
            }
        }
        ImGui::EndChild();
    }
}
}
