#include "ArcballCamera.hpp"

#include "math/definitions.hpp"
#include "math/transformations.hpp"
#include "math/trigonometric.hpp"

#include <algorithm>

namespace Zeus
{
ArcballCamera::ArcballCamera(
    float aspectRatio,
    Math::Vector3f position,
    Math::Vector3f target,
    Math::Vector3f up)
    : EditorCamera(CameraType::ARCBALL),
      m_position{ position },
      m_target{ target },
      m_up{ up },
      m_aspectRatio{ aspectRatio }
{
    m_projection = Math::perspective<float>(
        Math::radians(m_fieldOfView),
        m_aspectRatio,
        m_near,
        m_far);
}

void ArcballCamera::Reset()
{
    m_position = Math::Vector3f();
    m_target = Math::Vector3f();
    // m_direction;
    // m_up;
    // m_right;
    m_radius = 10.f;

    m_yaw = 0.f;
    m_pitch = 0.f;
    // m_roll = 0.f;

    m_fieldOfView = DEFAULT_FOV;
    m_near = DEFAULT_NEAR_PLANE_DISTANCE;
    m_far = DEFAULT_FAR_PLANE_DISTANCE;

    m_movementSpeed = DEFAULT_MOVEMENT_SPEED;
    m_mouseSensitivity = DEFAULT_MOUSE_SENSITIVITY;

    m_projection = Math::perspective<float>(
        Math::radians(m_fieldOfView),
        m_aspectRatio,
        m_near,
        m_far);
}

void ArcballCamera::Move(CameraMovement cameraMovement, float deltaTime)
{
    float velocity{ deltaTime * m_movementSpeed };

    switch (cameraMovement)
    {
    case CameraMovement::FORWARD:
        m_target = m_target + m_direction * velocity;
        break;
    case CameraMovement::BACK:
        m_target = m_target - m_direction * velocity;
        break;
    case CameraMovement::LEFT:
        m_target = m_target - m_right * velocity;
        break;
    case CameraMovement::RIGHT:
        m_target = m_target + m_right * velocity;
        break;
    case CameraMovement::UP:
        m_target = m_target + m_up * velocity;
        break;
    case CameraMovement::DOWN:
        m_target = m_target - m_up * velocity;
        break;
    }
}

void ArcballCamera::OnMouse(float xOffset, float yOffset, bool constrainPitch)
{
    m_yaw += xOffset * m_mouseSensitivity;
    m_pitch += yOffset * m_mouseSensitivity;

    if (constrainPitch)
    {
        m_pitch = std::clamp(m_pitch, -89.f, 89.f);
    }
}

void ArcballCamera::OnScroll(float yOffset)
{
    m_radius = std::clamp(m_radius - yOffset, 1.0f, 100.0f);
}

void ArcballCamera::Update()
{
    Math::Vector3f direction(
        m_radius * std::cos(Math::radians(m_yaw)) *
            std::cos(Math::radians(m_pitch)),
        m_radius * std::sin(Math::radians(m_pitch)),
        m_radius * std::sin(Math::radians(m_yaw)) *
            std::cos(Math::radians(m_pitch)));

    m_position = m_target - direction;

    m_direction = normalize(direction);
    m_right = normalize(cross(m_direction, WORLD_UP));
    m_up = normalize(cross(m_right, m_direction));

    m_view = lookAt(m_position, m_target, WORLD_UP);
    m_viewProjection = m_projection * m_view;
}

Math::Vector3f& ArcballCamera::GetPosition()
{
    return m_position;
}

const Math::Matrix4x4f& ArcballCamera::GetViewProjection() const
{
    return m_viewProjection;
}
}
