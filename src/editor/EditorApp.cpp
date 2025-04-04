#include "EditorApp.hpp"

#include <application/Application.hpp>
#include <application/entry_point.hpp>
#include <assets/AssetsManager.hpp>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <components/Renderable.hpp>
#include <core/Engine.hpp>
#include <events/Event.hpp>
#include <events/KeyEvent.hpp>
#include <events/MouseEvent.hpp>
#include <events/WindowEvent.hpp>
#include <imgui.h>
#include <input/Input.hpp>
#include <input/KeyCode.hpp>
#include <logging/logger.hpp>
#include <math/definitions.hpp>
#include <math/transformations.hpp>
#include <profiling/Profiler.hpp>
#include <rendering/Renderer_types.hpp>
#include <rhi/VkContext.hpp>
#include <rhi/vulkan/vulkan_dynamic_rendering.hpp>
#include <vulkan/vulkan_core.h>

#include <memory>

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
    : Application(specification)
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

    Event::Dispatcher.Register<KeyPressedEvent>(
        "Application::KeyPressedEvent",
        [this](const KeyPressedEvent& event) -> bool {
            return OnKeyPressed(event);
        });

    m_userInterface.Initialize(Window());
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

        if (m_userInterface.IsVisible())
            m_userInterface.Update();

        Engine::Renderer().BeginFrame();
        Engine::Renderer().Draw();

        if (m_userInterface.IsVisible())
            m_userInterface.Render();
        else
            Engine::Renderer().BlitToSwapchain();

        Engine::Renderer().Present();

        Profiler::End();
    }
}

void EditorApp::Shutdown()
{
    LOG_DEBUG("Shutting down Editor Application");

    VkContext::GetDevice().Wait();

    m_userInterface.Destroy();

    Engine::Shutdown();
}

void EditorApp::HandleKeyboard()
{
    float speed = 0.001f * (float)Profiler::s_frametimeDelta;
    if (Input::IsKeyDown(KeyCode::E))
        camera->Move(CameraMovement::UP, speed);

    if (Input::IsKeyDown(KeyCode::Q))
        camera->Move(CameraMovement::DOWN, speed);

    if (Input::IsKeyDown(KeyCode::W))
        camera->Move(CameraMovement::FORWARD, speed);

    if (Input::IsKeyDown(KeyCode::S))
        camera->Move(CameraMovement::BACK, speed);

    if (Input::IsKeyDown(KeyCode::A))
        camera->Move(CameraMovement::LEFT, speed);

    if (Input::IsKeyDown(KeyCode::D))
        camera->Move(CameraMovement::RIGHT, speed);
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

bool EditorApp::OnKeyPressed(const KeyPressedEvent& event)
{
    switch (event.keyCode)
    {
    case Zeus::KeyCode::Escape:
        glfwSetInputMode(
            reinterpret_cast<GLFWwindow*>(Window().GetHandle()),
            GLFW_CURSOR,
            GLFW_CURSOR_NORMAL);
        break;
    case Zeus::KeyCode::F11:
        if (!event.isRepeated)
            m_userInterface.SetVisible(true);
        break;
    case KeyCode::C:
        glfwSetInputMode(
            reinterpret_cast<GLFWwindow*>(Window().GetHandle()),
            GLFW_CURSOR,
            GLFW_CURSOR_DISABLED);
    default:
        break;
    }

    return true;
}
}
