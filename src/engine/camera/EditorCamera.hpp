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

enum class CameraType : std::uint8_t
{
    FREEFLY,
    ARCBALL,
};

class EditorCamera
{
public:
    inline EditorCamera(CameraType type) : m_cameraType{ type }
    {
    }

    virtual void Reset() = 0;

    virtual void Move(CameraMovement cameraMovement, float deltaTime) = 0;

    virtual void OnMouse(
        float xOffset,
        float yOffset,
        bool constrainPitch = true) = 0;

    virtual void OnScroll(float yOffset) = 0;

    virtual void Update() = 0;

    virtual Vector3f& GetPosition() = 0;

    virtual const Matrix4x4f& GetViewProjection() const = 0;

    inline constexpr bool IsType(CameraType cameraType) const
    {
        return m_cameraType == cameraType;
    };

    inline static constexpr float DEFAULT_MOVEMENT_SPEED{ 1.f };
    inline static constexpr float DEFAULT_MOUSE_SENSITIVITY{ 0.05f };
    inline static constexpr float DEFAULT_NEAR_PLANE_DISTANCE{ 10000.f };
    inline static constexpr float DEFAULT_FAR_PLANE_DISTANCE{ 0.1f };
    inline static constexpr float DEFAULT_FOV{ 45.f };
    inline static const Vector3f WORLD_UP{ Vector3f(0.f, 1.f, 0.f) };

protected:
    const CameraType m_cameraType;
};
}
