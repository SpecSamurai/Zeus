#include <math/definitions.hpp>

#include <vulkan/vulkan_core.h>

namespace Zeus
{
struct UniformBufferObject
{
    alignas(16) Matrix4x4f model;
    alignas(16) Matrix4x4f view;
    alignas(16) Matrix4x4f proj;
};

struct Vertex
{
    Vector3f position;
    Vector3f color;
    // Vector2f texCoord;
};
}
