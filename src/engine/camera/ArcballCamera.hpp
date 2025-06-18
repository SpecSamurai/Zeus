#pragma once

#include "EditorCamera.hpp"
#include "math/definitions.hpp"

#include <vulkan/vulkan_core.h>

namespace Zeus
{
class ArcballCamera : public EditorCamera
{
public:
    ArcballCamera(
        float aspectRatio,
        Math::Vector3f position = Math::Vector3f(),
        Math::Vector3f target = Math::Vector3f(),
        Math::Vector3f up = WORLD_UP);

    virtual void Reset() override;
    virtual void Move(CameraMovement cameraMovement, float deltaTime) override;
    virtual void Update() override;

    virtual void OnMouse(
        float xOffset,
        float yOffset,
        bool constrainPitch = true) override;
    virtual void OnScroll(float yOffset) override;

    virtual Math::Vector3f& GetPosition() override;
    virtual Math::Vector3f& GetDirection() override;
    virtual const Math::Matrix4x4f& GetViewProjection() const override;

private:
    Math::Matrix4x4f m_view;
    Math::Matrix4x4f m_projection;
    Math::Matrix4x4f m_viewProjection;

    Math::Vector3f m_position;
    Math::Vector3f m_target;
    Math::Vector3f m_direction;
    Math::Vector3f m_up;
    Math::Vector3f m_right;

    float m_radius{ 10.f };

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
