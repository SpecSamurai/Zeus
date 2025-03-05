#pragma once

#include "rhi/Buffer.hpp"
#include "rhi/Vertex.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <string_view>
#include <vector>

namespace Zeus
{
class Mesh
{
public:
    Mesh(std::string_view name = "");
    ~Mesh();

    void GetGeometry(
        uint32_t indexOffset,
        uint32_t indexCount,
        uint32_t vertexOffset,
        uint32_t vertexCount,
        std::vector<uint32_t>* indices,
        std::vector<Vertex>* vertices);

    void AddGeometry(
        const std::vector<Vertex>& vertices,
        const std::vector<std::uint32_t>& indices);

    void CreateBuffers();

    void Clear();

    const std::vector<Vertex>& GetVertices() const;
    const std::vector<std::uint32_t>& GetIndices() const;
    std::uint32_t GetVertexCount() const;
    std::uint32_t GetIndexCount() const;

    Buffer* GetVertexBuffer();
    Buffer* GetIndexBuffer();

    VkDeviceSize GetVerticesSize() const;
    VkDeviceSize GetIndicesSize() const;
    VkDeviceSize GetSize() const;
    std::string_view GetName() const;

private:
    std::vector<Vertex> m_vertices;
    std::vector<std::uint32_t> m_indices;

    Buffer* m_vertexBuffer;
    Buffer* m_indexBuffer;

    // Here or to Renderable
    // std::uint64_t m_vertexOffset;
    // std::uint64_t m_vertextCount;
    // std::uint64_t m_indexOffset;
    // std::uint64_t m_indexCount;

    // BoundingBox aabb;
    // ??? Parent weak_ptr
    // Mutex

    std::string_view m_name;
};
}
