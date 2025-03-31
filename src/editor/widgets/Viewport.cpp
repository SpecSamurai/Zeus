#include "Viewport.hpp"

#include <backends/imgui_impl_vulkan.h>
#include <core/Engine.hpp>

namespace Zeus
{
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

void Viewport::Update()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2.f, 2.f));

    if (ImGui::Begin("Viewport"))
    {
        float width{ static_cast<float>(m_image->GetWidth()) };
        float height{ static_cast<float>(m_image->GetHeight()) };

        ImGui::Image(
            reinterpret_cast<ImTextureID>(m_descriptorSet),
            ImVec2(width, height),
            ImVec2(0, 0),
            ImVec2(1, 1),
            ImVec4(1, 1, 1, 1), // tint
            ImColor(0, 0, 0, 0) // border
        );
    }
    ImGui::End();
    ImGui::PopStyleVar(2);
}
}
