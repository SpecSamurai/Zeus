#include "UserInterface.hpp"

#include "widgets/AssetBrowser.hpp"
#include "widgets/LogViewer.hpp"
#include "widgets/MenuBar.hpp"
#include "widgets/Properties.hpp"
#include "widgets/Settings.hpp"
#include "widgets/ShaderEditor.hpp"
#include "widgets/Viewport.hpp"
#include "widgets/WorldViewer.hpp"

#include <core/Engine.hpp>
#include <imgui.h>
#include <rhi/VkContext.hpp>

namespace Zeus
{
void UserInterface::Initialize(const Window& window)
{
    std::vector<VkDescriptorPoolSize> poolSizes{
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
        /*{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },*/
        /*{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },*/
        /*{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },*/
        /*{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },*/
        /*{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },*/
        /*{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },*/
        /*{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },*/
        /*{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },*/
        /*{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },*/
        /*{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },*/
        /*{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }*/
    };

    m_ImGuiDescriptorPool = DescriptorPool(
        "DescriptorPool_ImGui",
        2, /*1000,*/
        poolSizes,
        VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);

    ImGui::CreateContext();
    ImGuiIO& io{ ImGui::GetIO() };
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(
        reinterpret_cast<GLFWwindow*>(window.GetHandle()),
        true);

    auto& swapchain{ Engine::Renderer().GetSwapchain() };
    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance = VkContext::GetInstance();
    initInfo.PhysicalDevice = VkContext::GetDevice().GetPhysicalDevice();
    initInfo.Device = VkContext::GetLogicalDevice();
    initInfo.QueueFamily = VkContext::GetQueueFamily(QueueType::Graphics);
    initInfo.Queue = VkContext::GetQueue(QueueType::Graphics);
    // init_info.PipelineCache = YOUR_PIPELINE_CACHE;
    initInfo.DescriptorPool = m_ImGuiDescriptorPool.GetHandle();
    // init_info.Subpass = 0;
    initInfo.MinImageCount = swapchain.GetImageCount();
    initInfo.ImageCount = swapchain.GetImageCount();
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.UseDynamicRendering = true;

    initInfo.PipelineRenderingCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    initInfo.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
    initInfo.PipelineRenderingCreateInfo.pColorAttachmentFormats =
        &swapchain.GetFormat();

    initInfo.Allocator = allocationCallbacks.get();

    ImGui_ImplVulkan_Init(&initInfo);
    ImGui_ImplVulkan_CreateFontsTexture();

    // ImGui_ImplVulkan_DestroyFontUploadObjects();

    m_widgets.emplace_back(std::make_shared<MenuBar>());
    m_widgets.emplace_back(std::make_shared<Viewport>(this));
    m_widgets.emplace_back(std::make_shared<WorldViewer>());
    m_widgets.emplace_back(std::make_shared<AssetBrowser>());
    m_widgets.emplace_back(std::make_shared<LogViewer>());
    m_widgets.emplace_back(std::make_shared<Properties>());
    m_widgets.emplace_back(std::make_shared<Settings>(this));
    m_widgets.emplace_back(std::make_shared<ShaderEditor>());

    for (auto& widget : m_widgets)
    {
        widget->Initialize();
    }
}

void UserInterface::Destroy()
{
    for (auto& widget : m_widgets)
    {
        widget->Destroy();
    }

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    m_ImGuiDescriptorPool.Destroy();
}

void UserInterface::Update()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2.f, 2.f));

    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

    ImGui::ShowDemoWindow();

    for (auto& widget : m_widgets)
    {
        if (widget->IsVisible())
            widget->Update();
    }

    ImGui::PopStyleVar(2);

    ImGui::Render();
}

void UserInterface::Render()
{
    auto& cmd{ Engine::Renderer().GetCommandBuffer() };
    auto& swapchain{ Engine::Renderer().GetSwapchain() };
    auto& renderOutputColor{
        Engine::Renderer().GetRenderTarget(RenderTarget::RENDER_OUTPUT_COLOR),
    };

    cmd.TransitionImageLayout(
        renderOutputColor.GetHandle(),
        renderOutputColor.GetFormat(),
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    swapchain.SetLayout(cmd, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    VkRenderingAttachmentInfo colorAttachmentInfo2 = createColorAttachmentInfo(
        swapchain.GetImageView(),
        swapchain.GetLayout());

    cmd.BeginRendering(swapchain.GetExtent(), 1, &colorAttachmentInfo2);

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd.GetHandle());

    cmd.EndRendering();
}

bool UserInterface::IsVisible() const
{
    return m_isVisible;
}

void UserInterface::SetVisible(bool value)
{
    m_isVisible = value;
}
}
