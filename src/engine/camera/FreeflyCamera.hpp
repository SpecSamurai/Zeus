#pragma once

#include "EditorCamera.hpp"
#include "math/definitions.hpp"

namespace Zeus
{
class FreeflyCamera : public EditorCamera
{
public:
    FreeflyCamera(
        float aspectRatio,
        Vector3f position = Vector3f(),
        Vector3f up = WORLD_UP);

    virtual void Reset() override;
    virtual void Move(CameraMovement cameraMovement, float deltaTime) override;
    virtual void Update() override;

    virtual void OnMouse(
        float xOffset,
        float yOffset,
        bool constrainPitch = true) override;
    virtual void OnScroll(float yOffset) override;

    virtual Vector3f& GetPosition() override;
    virtual const Matrix4x4f& GetViewProjection() const override;

private:
    Matrix4x4f m_view;
    Matrix4x4f m_projection;
    Matrix4x4f m_viewProjection;

    Vector3f m_position;
    Vector3f m_target;
    Vector3f m_direction;
    Vector3f m_up;
    Vector3f m_right;

    // Euler angles
    float m_yaw{ 0.f };
    float m_pitch{ 0.f };
    // float m_roll{ 0.f };

    float m_aspectRatio;
    float m_fieldOfView{ DEFAULT_FOV };
    float m_near{ DEFAULT_NEAR_PLANE_DISTANCE };
    float m_far{ DEFAULT_FAR_PLANE_DISTANCE };

    float m_movementSpeed{ DEFAULT_MOVEMENT_SPEED };
    float m_mouseSensitivity{ DEFAULT_MOUSE_SENSITIVITY };
};
}
