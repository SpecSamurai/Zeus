#include "UserInterface.hpp"

#include "imgui.h"
#include "imgui_internal.h"
#include "widgets/Demo.hpp"
#include "widgets/MenuBar.hpp"
#include "widgets/Profiler.hpp"
#include "widgets/Viewport.hpp"

#include <core/Engine.hpp>
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
        2, /*1000,*/
        poolSizes,
        VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        "DescriptorPool_ImGui");

    ImGui::CreateContext();
    ImGuiIO& io{ ImGui::GetIO() };
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.DisplaySize = ImVec2(1400, 1020);
    // io.DeltaTime

    ImGui::StyleColorsDark();

    // todo refactor gethandle to return type template
    ImGui_ImplGlfw_InitForVulkan(
        reinterpret_cast<GLFWwindow*>(window.GetHandle()),
        true);

    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance = VkContext::GetInstance();
    initInfo.PhysicalDevice = VkContext::GetDevice().GetPhysicalDevice();
    initInfo.Device = VkContext::GetLogicalDevice();
    initInfo.QueueFamily = VkContext::GetQueueFamily(QueueType::Graphics);
    initInfo.Queue = VkContext::GetQueue(QueueType::Graphics);
    // init_info.PipelineCache = YOUR_PIPELINE_CACHE;
    initInfo.DescriptorPool = m_ImGuiDescriptorPool.GetHandle();
    // init_info.Subpass = 0;
    initInfo.MinImageCount = 3; // VkContext.GetSwapchain().imageCount;
    initInfo.ImageCount = 3;    // VkContext.GetSwapchain().imageCount;
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.UseDynamicRendering = true;

    initInfo.PipelineRenderingCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    initInfo.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
    initInfo.PipelineRenderingCreateInfo.pColorAttachmentFormats =
        &Engine::Renderer().m_swapchain.GetFormat();
    // &vkContext.GetSwapchain().imageFormat;

    initInfo.Allocator = allocationCallbacks.get();

    ImGui_ImplVulkan_Init(&initInfo);
    ImGui_ImplVulkan_CreateFontsTexture();

    // ImGui_ImplVulkan_DestroyFontUploadObjects();

    m_widgets.emplace_back(std::make_shared<MenuBar>());
    m_widgets.emplace_back(std::make_shared<Viewport>());
    m_widgets.emplace_back(std::make_shared<Profiler>());
    m_widgets.emplace_back(std::make_shared<Demo>());

    for (auto& widget : m_widgets)
    {
        widget->Initialize();
    }
}

void UserInterface::Destroy()
{
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

void UserInterface::Render(CommandBuffer& cmd)
{
    Engine::Renderer().m_swapchain.SetLayout(
        cmd,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    VkRenderingAttachmentInfo colorAttachmentInfo2 = createColorAttachmentInfo(
        Engine::Renderer().m_swapchain.GetImageView(),
        Engine::Renderer().m_swapchain.GetLayout());

    cmd.BeginRendering(
        Engine::Renderer().m_swapchain.GetExtent(),
        1,
        &colorAttachmentInfo2);

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd.GetHandle());

    cmd.EndRendering();

    Engine::Renderer().m_swapchain.SetLayout(
        cmd,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
}
}
