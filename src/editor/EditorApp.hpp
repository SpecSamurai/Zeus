#pragma once

#include "camera/EditorCamera.hpp"
#include "camera/camera_utils.hpp"
#include <application/Application.hpp>
#include <events/MouseEvent.hpp>
#include <events/WindowEvent.hpp>
#include <window/Window.hpp>

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

private:
    inline static std::unique_ptr<EditorCamera> camera =
        createFreeflyEditorCamera(1440.f / 1080.f);

    inline static struct MousePosition
    {
        float x;
        float y;
    } m_lastMousePosition;
    inline static bool m_isMouseReleased{ true };
};
}
