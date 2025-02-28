#include "EditorApp.hpp"

#include <application/Application.hpp>
#include <application/entry_point.hpp>
#include <assets/AssetsManager.hpp>
#include <core/Engine.hpp>
#include <events/Event.hpp>
#include <events/MouseEvent.hpp>
#include <events/WindowEvent.hpp>
#include <input/Input.hpp>
#include <input/KeyCode.hpp>
#include <logging/logger.hpp>
#include <profiling/Profiler.hpp>

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

    Engine::Renderer().DrawTriangle(
        Math::Vector3f(0.0f, -0.5f, 0.0f),
        Math::Vector3f(0.5, 0.5, 0),
        Math::Vector3f(-0.5, 0.5, 0),
        Math::Colors::GREEN);

    Engine::Renderer().DrawRectangle(
        Math::Vector3f(0.0f, 0.0f, 0.0f),
        Math::Vector3f(0.5, 0.0, 0),
        Math::Vector3f(0.5, 0.5, 0),
        Math::Vector3f(0, 0.5, 0),
        Math::Colors::BLUE);

    Engine::Renderer().DrawLine(
        Math::Vector3f(0.0f, 0.0f, 0.0f),
        Math::Vector3f(0.5f, -0.3f, 0.3f),
        Math::Colors::MAGENTA,
        Math::Colors::CYAN);

    Engine::Renderer().m_linesVertexBuffer.Update(
        Engine::Renderer().m_lines.data(),
        Engine::Renderer().m_lines.size() * sizeof(Vertex_PositionColor));

    auto asset =
        AssetsManager::GetObjLoader()->Load("../../models/viking_room.obj");

    Engine::Renderer().m_meshVertexBuffer.Update(
        asset->mesh->m_vertices.data(),
        asset->mesh->m_vertices.size() * sizeof(Vertex));
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

        // https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/staying_within_budget.html
        // Make sure to call vmaSetCurrentFrameIndex() every frame.
        // Budget is queried from Vulkan inside of it to avoid overhead of
        // querying it with every allocation.
        // vmaSetCurrentFrameIndex(
        //     vulkan_memory_allocator::allocator,
        //     static_cast<uint32_t>(frame_count));

        HandleKeyboard();
        camera->Update();

        /*Engine::GetRenderer().SetCamera(camera->GetViewProjection());*/
        Engine::Renderer().SetCameraProjection(camera->GetViewProjection());
        Engine::Update();

        Profiler::End();
    }
}

void EditorApp::Shutdown()
{
    LOG_DEBUG("Shutting down Editor Application");

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
}
