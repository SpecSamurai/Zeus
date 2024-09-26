#pragma once

#include "math/definitions.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>

namespace Zeus
{
enum class CameraMovement : std::uint8_t
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    FORWARD,
    BACK,
};

enum class CameraProjection : std::uint8_t
{
    ORTHOGRAPHIC,
    PERSPECTIVE
};

enum class CameraType : std::uint8_t
{
    FREE_FLY,
    ARC_BALL,
};

class Camera
{
public:
    Camera(CameraType type) : m_cameraType{ type }
    {
    }

    virtual void ProcessKeyboard(
        CameraMovement cameraMovement,
        float deltaTime) = 0;

    virtual void ProcessMouseMovement(
        float xOffset,
        float yOffset,
        bool constrainPitch = true) = 0;

    virtual void ProcessMouseScroll(float yOffset) = 0;

    virtual void Update() = 0;

    inline void SetTarget(Vector3f target)
    {
        m_target = target;
    }

    inline void SetProjection(
        Matrix4x4f projection,
        CameraProjection cameraProjection)
    {
        m_projection = projection;
        m_cameraProjection = cameraProjection;
    }

    inline void SetMovementSpeed(float movementSpeed)
    {
        m_movementSpeed = movementSpeed;
    }

    inline void SetMouseSensitivity(float mouseSensitivity)
    {
        m_mouseSensitivity = mouseSensitivity;
    }

    inline Matrix4x4f GetViewProjection()
    {
        return m_projection * m_view;
    };

protected:
    Matrix4x4f m_view;
    Matrix4x4f m_projection;

    Vector3f m_position;
    Vector3f m_target;

    float m_movementSpeed{ 1.0f };
    float m_mouseSensitivity{ 0.05f };

    const CameraType m_cameraType;
    CameraProjection m_cameraProjection;
};
}
