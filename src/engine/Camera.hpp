#pragma once

#include "math/definitions.hpp"
#include "math/geometric.hpp"
#include "math/transformations.hpp"
#include "math/trigonometric.hpp"

namespace Zeus
{
enum class CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera
{
public:
    Camera(
        Vector3f position = Vector3f(),
        Vector3f up = Vector3f(0.0f, 1.0f, 0.0f),
        float yaw = 0.f,
        float pitch = 0.f)
        : position{ position },
          up{ up },
          pitch{ pitch },
          yaw{ yaw }
    {
        Update();
    }

    void Update()
    {
        Vector3f tmpFront;

        tmpFront.x = std::cos(radians(yaw)) * std::cos(radians(pitch));
        tmpFront.y = std::sin(radians(pitch));
        tmpFront.z = std::sin(radians(yaw)) * std::cos(radians(pitch));

        front = normalize(tmpFront);

        right = normalize(cross(front, up));
        // up = normalize(cross(right, front));

        // target = position + front;
    }

    Matrix4x4f getViewMatrix()
    {
        return lookAt(position, position + front, up);
    }

    void processKeyboard(CameraMovement cameraMovement, float deltaTime)
    {
        float velocity{ deltaTime * movementSpeed };

        switch (cameraMovement)
        {
        case CameraMovement::FORWARD:
            position = position + front * velocity;
            return;
        case CameraMovement::BACKWARD:
            position = position - front * velocity;
            return;
        case CameraMovement::LEFT:
            position = position - right * velocity;
            return;
        case CameraMovement::RIGHT:
            position = position + right * velocity;
            return;
        }
    }

    void processMouseMovement(
        float xOffset,
        float yOffset,
        bool constraintPitch = true)
    {
        yaw += xOffset * sensitivity;
        pitch += yOffset * sensitivity;

        if (constraintPitch)
        {
            if (pitch > 90.f)
                pitch = 89.f;

            if (pitch < -89.f)
                pitch = -89.0f;
        }
    }

    void setZoom(float yOffset)
    {
        zoom -= yOffset;

        if (zoom < 1.f)
            zoom = 1.f;
        if (zoom > 45.f)
            zoom = 45.f;
    }

    void setRotationSpeed(float rotationSpeed)
    {
        this->rotationSpeed = rotationSpeed;
    }

    void setMovementSpeed(float movementSpeed)
    {
        this->movementSpeed = movementSpeed;
    }

private:
public:
    Matrix4x4f viewMatrix;

    Vector3f position;
    Vector3f front;
    Vector3f right;
    Vector3f up;

    // Euler angles
    float pitch{ 0.f };
    float yaw{ 0.f };
    float roll{ 0.f };

    float movementSpeed{ 1.0f };
    float rotationSpeed{ 1.0f };
    float sensitivity{ 0.05f };
    float zoom{ 45.f };
};
}
