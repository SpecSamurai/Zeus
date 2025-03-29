#include "EditorApp.hpp"

#include "widgets/Demo.hpp"

#include <application/Application.hpp>
#include <application/entry_point.hpp>
#include <assets/AssetsManager.hpp>
#include <components/Renderable.hpp>
#include <core/Engine.hpp>
#include <events/Event.hpp>
#include <events/MouseEvent.hpp>
#include <events/WindowEvent.hpp>
#include <input/Input.hpp>
#include <input/KeyCode.hpp>
#include <logging/logger.hpp>
#include <math/definitions.hpp>
#include <math/transformations.hpp>
#include <profiling/Profiler.hpp>
#include <rendering/Renderer_types.hpp>
#include <rhi/VkContext.hpp>
#include <rhi/vulkan/vulkan_dynamic_rendering.hpp>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <vulkan/vulkan_core.h>

#include <memory>
#include <vector>

namespace Zeus
{
Application* CreateApplication(CommandLineArgs args)
{
    ApplicationSpecification specification{
        .name = "Zeus Editor",
        .commandLineArgs = args,
        .windowWidth = 1440,
        .windowHeight = 1080,
    };

    return new EditorApp(specification);
}

EditorApp::EditorApp(const ApplicationSpecification& specification)
    : Application(specification),
      m_widgets{ std::make_shared<Demo>() }
{
}

void EditorApp::Initialize()
{
    LOG_DEBUG("Initializing Editor Application");

    Engine::Initialize(Window());

    Event::Dispatcher.Register<MouseMovedEvent>(
        "Application::MouseMovedEvent",
        [this](const MouseMovedEvent& event) -> bool {
            return OnMouseMoved(event);
        });

    InitImGui();
}

void EditorApp::Run()
{
    while (IsRunning())
    {
        Profiler::Begin();

        Window().Update();

        if (IsMinimized())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        HandleKeyboard();
        camera->Update();

        Engine::Renderer().SetCameraProjection(camera->GetViewProjection());
        Engine::Update();

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        for (auto& widget : m_widgets)
        {
            widget->Update();
        }

        ImGui::Render();

        auto& cmd{ Engine::Renderer().CurrentFrame().graphicsCommandBuffer };

        Engine::Renderer().m_swapchain.SetLayout(
            cmd,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        VkRenderingAttachmentInfo colorAttachmentInfo2 =
            createColorAttachmentInfo(
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
        cmd.End();

        Engine::Renderer().m_swapchain.Present(cmd.GetHandle());

        Profiler::End();
    }
}

void EditorApp::Shutdown()
{
    LOG_DEBUG("Shutting down Editor Application");

    VkContext::GetDevice().Wait();

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    m_ImGuiDescriptorPool.Destroy();

    Engine::Shutdown();
}

void EditorApp::HandleKeyboard()
{
    auto speedd = 0.0001f;
    if (Input::IsKeyDown(KeyCode::E))
        camera->Move(CameraMovement::UP, speedd);

    if (Input::IsKeyDown(KeyCode::Q))
        camera->Move(CameraMovement::DOWN, speedd);

    if (Input::IsKeyDown(KeyCode::W))
        camera->Move(CameraMovement::FORWARD, speedd);

    if (Input::IsKeyDown(KeyCode::S))
        camera->Move(CameraMovement::BACK, speedd);

    if (Input::IsKeyDown(KeyCode::A))
        camera->Move(CameraMovement::LEFT, speedd);

    if (Input::IsKeyDown(KeyCode::D))
        camera->Move(CameraMovement::RIGHT, speedd);

    if (Input::IsKeyDown(KeyCode::C))
        glfwSetInputMode(
            reinterpret_cast<GLFWwindow*>(Window().GetHandle()),
            GLFW_CURSOR,
            GLFW_CURSOR_DISABLED);

    if (Input::IsKeyDown(KeyCode::V))
        glfwSetInputMode(
            reinterpret_cast<GLFWwindow*>(Window().GetHandle()),
            GLFW_CURSOR,
            GLFW_CURSOR_NORMAL);

    // if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    //     glfwSetWindowShouldClose(window, GLFW_TRUE);
}

bool EditorApp::OnMouseMoved(const MouseMovedEvent& event)
{
    if (camera->IsType(CameraType::FREEFLY) &&
        glfwGetInputMode(
            reinterpret_cast<GLFWwindow*>(Window().GetHandle()),
            GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
    {
        return true;
    }

    if (!glfwGetMouseButton(
            reinterpret_cast<GLFWwindow*>(Window().GetHandle()),
            GLFW_MOUSE_BUTTON_LEFT))
    {
        m_isMouseReleased = true;
        return true;
    }

    float xPosition{ static_cast<float>(event.position.x) };
    float yPosition{ static_cast<float>(event.position.y) };

    if (m_isMouseReleased)
    {
        m_lastMousePosition.x = xPosition;
        m_lastMousePosition.y = yPosition;
        m_isMouseReleased = false;
    }

    float xOffset{ xPosition - m_lastMousePosition.x };
    float yOffset{ m_lastMousePosition.y - yPosition };

    m_lastMousePosition.x = xPosition;
    m_lastMousePosition.y = yPosition;

    camera->OnMouse(xOffset, yOffset);

    return true;
}

void EditorApp::InitImGui()
{
    std::vector<VkDescriptorPoolSize> poolSizes{
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };

    m_ImGuiDescriptorPool = DescriptorPool(
        1000,
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
        reinterpret_cast<GLFWwindow*>(Window().GetHandle()),
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
}
}
