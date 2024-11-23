#pragma once

#include "math/definitions.hpp"

namespace Zeus
{
struct Vertex_Position
{
    Vector3f position;
};

struct Vertex_PositionTexture
{
    Vector3f position;
    Vector2f texture;
};

struct Vertex_PositionColor
{
    Vector3f position;
    Color color;
};

// UV parameters are interleaved is due to alignement limitations
struct Vertex
{
    Vector3f position;
    float uvX;
    Vector3f normal;
    float uvY;
    Color color;
};
}
