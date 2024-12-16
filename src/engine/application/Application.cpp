#include "Application.hpp"

#include "GLFW/glfw3.h"
#include "core/Engine.hpp"
#include "events/Event.hpp"
#include "events/MouseEvent.hpp"
#include "events/WindowEvent.hpp"
#include "input/Input.hpp"
#include "input/KeyCode.hpp"
#include "logging/logger.hpp"
#include "math/definitions.hpp"
#include "profiling/Profiler.hpp"
#include "rendering/Renderer2.hpp"
#include "window/Window.hpp"

#include <cassert>
#include <thread>

namespace Zeus
{
Application* Application::s_instance{ nullptr };
Renderer2* renderer2;

Application::Application(const ApplicationSpecification& specification)
    : m_window(WindowProperties{
          .title = specification.name,
          .width = specification.windowWidth,
          .height = specification.windowHeight,
      })
{
    assert(!s_instance && "Application already created.");
    s_instance = this;
}

void Application::Initialize()
{
    Event::Dispatcher.Register<WindowClosedEvent>(
        "Application::WindowClosedEvent",
        [this](const WindowClosedEvent& event) -> bool {
            return OnWindowClosed(event);
        });

    Event::Dispatcher.Register<WindowResizedEvent>(
        "Application::WindowResizedEvent",
        [this](const WindowResizedEvent& event) -> bool {
            return OnWindowResized(event);
        });

    Event::Dispatcher.Register<MouseMovedEvent>(
        "Application::MouseMovedEvent",
        [this](const MouseMovedEvent& event) -> bool {
            return OnMouseMoved(event);
        });

    m_window.Initialize();

    Engine::Initialize(m_window);
    renderer2 = new Renderer2(m_window);
    renderer2->Initialize();
    // InputManager::Init((GLFWwindow*)m_window.GetHandle());
    // uiManager.Init(m_window, m_vkContext);
}

void Application::Run()
{
    m_running = true;
    // while (!glfwWindowShouldClose((GLFWwindow*)m_window.GetHandle()))
    while (m_running)
    {
        Profiler::Begin();

        m_window.Update();

        if (m_minimized)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        // https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/staying_within_budget.html
        // Make sure to call vmaSetCurrentFrameIndex() every frame.
        // Budget is queried from Vulkan inside of it to avoid overhead of
        // querying it with every allocation.
        // vmaSetCurrentFrameIndex(
        //     vulkan_memory_allocator::allocator,
        //     static_cast<uint32_t>(frame_count));

        // if (m_renderer.ResizeRequired())
        // {
        //     VkExtent2D e{ m_window.GetWidth(), m_window.GetHeight() };
        //     m_renderer.ResizeDrawObjects(e);

        // uiManager.ConfigureFrame();

        // imgui commands
        // updateScene();

        HandleKeyboard();
        camera->Update();
        // Engine::GetRenderer().SetCamera(camera->GetViewProjection());
        // Engine::GetRenderer().Draw();

        auto a{ camera->GetViewProjection() };
        renderer2->m_frameDataBuffer.Update(
            &a,
            sizeof(camera->GetViewProjection()));

        renderer2->Update();

        Profiler::End();
    }
}

void Application::Shutdown()
{
    LOG_DEBUG("Shutting down application");

    renderer2->Destroy();

    Engine::Shutdown();
    m_window.Destroy();
}

bool Application::OnWindowClosed(
    [[maybe_unused]] const WindowClosedEvent& event)
{
    m_running = false;
    return true;
}

bool Application::OnWindowResized(const WindowResizedEvent& event)
{
    m_minimized = event.width == 0 || event.height == 0;
    return true;
}

void Application::HandleKeyboard()
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
            reinterpret_cast<GLFWwindow*>(m_window.GetHandle()),
            GLFW_CURSOR,
            GLFW_CURSOR_DISABLED);

    if (Input::IsKeyDown(KeyCode::V))
        glfwSetInputMode(
            reinterpret_cast<GLFWwindow*>(m_window.GetHandle()),
            GLFW_CURSOR,
            GLFW_CURSOR_NORMAL);

    // if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    //     glfwSetWindowShouldClose(window, GLFW_TRUE);
}

bool Application::OnMouseMoved(const MouseMovedEvent& event)
{
    if (camera->IsType(CameraType::FREEFLY) &&
        glfwGetInputMode(
            reinterpret_cast<GLFWwindow*>(m_window.GetHandle()),
            GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
    {
        return true;
    }

    if (!glfwGetMouseButton(
            reinterpret_cast<GLFWwindow*>(m_window.GetHandle()),
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
}
