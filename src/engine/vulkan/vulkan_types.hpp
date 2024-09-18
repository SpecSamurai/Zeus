#pragma once

#include "DeletionQueue.hpp"
#include "DynamicDescriptorAllocator.hpp"
#include "math/definitions.hpp"
#include "vulkan_buffer.hpp"
#include "vulkan_image.hpp"

#include <vulkan/vulkan.h>

namespace Zeus
{
namespace Colors
{
const static Vector4f RED{ Vector4f(1.f, 0.f, 0.f, 1.f) };
const static Vector4f GREEN{ Vector4f(0.f, 1.f, 0.f, 1.f) };
}

struct FrameData
{
    DeletionQueue deletionQueue;

    VkCommandPool commandPool;
    VkCommandBuffer mainCommandBuffer;

    VkSemaphore imageAcquiredSemaphore;
    VkSemaphore renderCompleteSemaphore;

    VkFence renderFence;

    DynamicDescriptorAllocator frameDescriptors;

    // TODO: conisider adding draw resources per frame
    // Image Backbuffer;
};

// UV parameters are interleaved is due to alignement limitations
struct Vertex
{
    Vector3f position;
    float uvX;
    Vector3f normal;
    float uvY;
    Vector4f color;
    // static VertexInputDescription get_vertex_description();
};

struct SceneData
{
    alignas(16) Matrix4x4f view;
    alignas(16) Matrix4x4f projection;
    alignas(16) Matrix4x4f view_projection;
    Vector4f ambientColor;
    Vector4f sunlightDirection; // w is strength
    Vector4f sunlightColor;
};

struct MeshBuffers
{
    Buffer indexBuffer;
    Buffer vertexBuffer;
    VkDeviceAddress vertexBufferAddress;
};

struct MeshPushConstants
{
    Matrix4x4f modelMatrix;
    VkDeviceAddress vertexBufferAddress;
};
}