#pragma once

#include "Vertex.hpp"
#include "rhi/Buffer.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <mutex>
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
        std::uint32_t vertexOffset,
        std::uint32_t vertexCount,
        std::uint32_t indexOffset,
        std::uint32_t indexCount,
        std::vector<Vertex>* outVertices,
        std::vector<std::uint32_t>* outIndices);

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

    std::string_view m_name;

    std::mutex m_mutex;
};
}
