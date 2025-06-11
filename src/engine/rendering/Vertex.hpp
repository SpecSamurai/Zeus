#pragma once

#include "math/definitions.hpp"

namespace Zeus
{
struct Vertex_Position
{
    Math::Vector3f position;
};

struct Vertex_PositionColor
{
    Math::Vector3f position;
    Math::Color color;
};

struct Vertex_PositionTexture
{
    Math::Vector3f position;
    Math::Vector2f texture;
};

// UV parameters are interleaved is due to alignement limitations
// Tangent
struct Vertex
{
    Math::Vector3f position;
    float uvX;
    Math::Vector3f normal;
    float uvY;
    Math::Color color;
};
}
