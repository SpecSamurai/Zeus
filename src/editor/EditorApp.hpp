#pragma once

#include "UserInterface.hpp"

#include <application/Application.hpp>
#include <camera/EditorCamera.hpp>
#include <camera/FreeflyCamera.hpp>
#include <events/MouseEvent.hpp>
#include <events/WindowEvent.hpp>
#include <rhi/DescriptorPool.hpp>
#include <window/Window.hpp>

#include <memory>

namespace Zeus
{
class EditorApp : public Application
{
public:
    EditorApp(const ApplicationSpecification& specification);

    void Initialize() override;
    void Run() override;
    void Shutdown() override;

private:
    bool OnMouseMoved(const MouseMovedEvent& event);

    void HandleKeyboard();

    void InitImGui();

private:
    UserInterface m_userInterface;

    inline static std::unique_ptr<EditorCamera> camera =
        std::make_unique<FreeflyCamera>(FreeflyCamera(1440.f / 1080.f));

    inline static struct MousePosition
    {
        float x;
        float y;
    } m_lastMousePosition;
    inline static bool m_isMouseReleased{ true };
};
}
