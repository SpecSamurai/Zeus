#pragma once

#include "widgets/Widget.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <rhi/CommandBuffer.hpp>
#include <rhi/DescriptorPool.hpp>
#include <rhi/vulkan/vulkan_dynamic_rendering.hpp>
#include <vulkan/vulkan_core.h>
#include <window/Window.hpp>

namespace Zeus
{
class UserInterface
{
public:
    void Initialize(const Window& window);
    void Destroy();
    void Update();
    void Render(CommandBuffer& cmd);

private:
    std::vector<std::shared_ptr<Widget>> m_widgets{};
    DescriptorPool m_ImGuiDescriptorPool;
};
}
