#include "FreeflyCamera.hpp"

#include "math/definitions.hpp"
#include "math/transformations.hpp"
#include "math/trigonometric.hpp"

#include <algorithm>

namespace Zeus
{
FreeflyCamera::FreeflyCamera(float aspectRatio, Vector3f position, Vector3f up)
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

void FreeflyCamera::Reset()
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

void FreeflyCamera::Move(CameraMovement cameraMovement, float deltaTime)
{
    float velocity{ deltaTime * m_movementSpeed };

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
        m_position = m_position - m_up * velocity;
        break;
    }
};

void FreeflyCamera::OnMouse(float xOffset, float yOffset, bool constrainPitch)
{
    m_yaw += xOffset * m_mouseSensitivity;
    m_pitch += yOffset * m_mouseSensitivity;

    if (constrainPitch)
    {
        m_pitch = std::clamp(m_pitch, -89.f, 89.f);
    }
};

void FreeflyCamera::OnScroll(float yOffset)
{
    if (yOffset > 0)
        m_position = m_position + m_direction;
    else if (yOffset < 0)
        m_position = m_position - m_direction;
};

void FreeflyCamera::Update()
{
    Vector3f direction(
        std::cos(radians(m_yaw)) * std::cos(radians(m_pitch)),
        std::sin(radians(m_pitch)),
        std::sin(radians(m_yaw)) * std::cos(radians(m_pitch)));

    m_direction = normalize(direction);
    m_right = normalize(cross(m_direction, WORLD_UP));
    m_up = normalize(cross(m_right, m_direction));

    m_target = m_position + m_direction;

    m_view = lookAt(m_position, m_target, WORLD_UP);
    m_viewProjection = m_projection * m_view;
};

Vector3f& FreeflyCamera::GetPosition()
{
    return m_position;
};

const Matrix4x4f& FreeflyCamera::GetViewProjection() const
{
    return m_viewProjection;
};
}
