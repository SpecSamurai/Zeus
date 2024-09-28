#pragma once

#include "EditorCamera.hpp"

#include <math/definitions.hpp>
#include <math/transformations.hpp>
#include <math/trigonometric.hpp>

#include <vulkan/vulkan_core.h>

#include <algorithm>

namespace Zeus
{
class FreeflyCamera : public EditorCamera
{
public:
    FreeflyCamera(
        float aspectRatio,
        Vector3f position = Vector3f(),
        Vector3f up = WORLD_UP)
        : EditorCamera(CameraType::FREEFLY),
          m_position{ position },
          m_up{ up },
          m_aspectRatio{ aspectRatio }
    {
        m_projection = perspective<float>(
            radians(m_fieldOfView),
            m_aspectRatio,
            m_near,
            m_far);
    }

    void Reset()
    {
        m_position = Vector3f();
        m_target = Vector3f();
        // m_direction;
        // m_up;
        // m_right;

        m_yaw = 0.f;
        m_pitch = 0.f;
        // m_roll = 0.f;

        m_fieldOfView = DEFAULT_FOV;
        m_near = DEFAULT_NEAR_PLANE_DISTANCE;
        m_far = DEFAULT_FAR_PLANE_DISTANCE;

        m_movementSpeed = DEFAULT_MOVEMENT_SPEED;
        m_mouseSensitivity = DEFAULT_MOUSE_SENSITIVITY;

        m_projection = perspective<float>(
            radians(m_fieldOfView),
            m_aspectRatio,
            m_near,
            m_far);
    };

    void Move(CameraMovement cameraMovement, float deltaTime)
    {
        float velocity = deltaTime * m_movementSpeed;

        switch (cameraMovement)
        {
        case CameraMovement::FORWARD:
            m_position = m_position + m_direction * velocity;
            break;
        case CameraMovement::BACK:
            m_position = m_position - m_direction * velocity;
            break;
        case CameraMovement::LEFT:
            m_position = m_position - m_right * velocity;
            break;
        case CameraMovement::RIGHT:
            m_position = m_position + m_right * velocity;
            break;
        case CameraMovement::UP:
            m_position = m_position + m_up * velocity;
            break;
        case CameraMovement::DOWN:
            m_position = m_position + m_up * velocity;
            break;
        }
    };

    void OnMouse(float xOffset, float yOffset, bool constrainPitch = true)
    {
        m_yaw += xOffset * m_mouseSensitivity;
        m_pitch += yOffset * m_mouseSensitivity;

        if (constrainPitch)
        {
            m_pitch = std::clamp(m_pitch, -89.f, 89.f);
        }
    };

    void OnScroll(float yOffset)
    {
        if (yOffset > 0)
            m_position = m_position + m_direction;
        else if (yOffset < 0)
            m_position = m_position - m_direction;
    };

    void Update()
    {
        Vector3f direction;
        direction.x = std::cos(radians(m_yaw)) * std::cos(radians(m_pitch));
        direction.y = std::sin(radians(m_pitch));
        direction.z = std::sin(radians(m_yaw)) * std::cos(radians(m_pitch));

        m_direction = normalize(direction);
        m_right = normalize(cross(m_direction, WORLD_UP));
        m_up = normalize(cross(m_right, m_direction));

        m_target = m_position + m_direction;

        m_view = lookAt(m_position, m_target, WORLD_UP);
        m_viewProjection = m_projection * m_view;
    };

    virtual Vector3f& GetPosition()
    {
        return m_position;
    };

    const Matrix4x4f& GetViewProjection() const
    {
        return m_viewProjection;
    };

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
