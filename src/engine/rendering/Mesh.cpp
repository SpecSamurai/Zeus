#include "Mesh.hpp"

#include "rhi/Buffer.hpp"

#include <cassert>
#include <cstdint>
#include <format>

namespace Zeus
{
Mesh::Mesh(std::string_view name)
    : m_vertices{},
      m_indices{},
      m_vertexBuffer{ nullptr },
      m_indexBuffer{ nullptr },
      m_name{ name }
{
}

Mesh::~Mesh()
{
    if (m_vertexBuffer)
    {
        m_vertexBuffer->Destroy();
        delete m_vertexBuffer;
        m_vertexBuffer = nullptr;
    }

    if (m_indexBuffer)
    {
        m_indexBuffer->Destroy();
        delete m_indexBuffer;
        m_indexBuffer = nullptr;
    }
}

void Mesh::GetGeometry(
    std::uint32_t indexOffset,
    std::uint32_t indexCount,
    std::uint32_t vertexOffset,
    std::uint32_t vertexCount,
    std::vector<std::uint32_t>* indices,
    std::vector<Vertex>* vertices)
{
}

void Mesh::AddGeometry(
    const std::vector<Vertex>& vertices,
    const std::vector<std::uint32_t>& indices)
{
    // mutex
    m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());
    m_indices.insert(m_indices.end(), indices.begin(), indices.end());
}

void Mesh::CreateBuffers()
{
    assert(GetVerticesSize() && GetIndicesSize() && "Invalid size");

    m_vertexBuffer = new Buffer(
        std::format("Buffer_Vertex_{}", m_name),
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
        GetVerticesSize(),
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    m_indexBuffer = new Buffer(
        std::format("Buffer_Index_{}", m_name),
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        GetIndicesSize(),
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    m_vertexBuffer->Update(m_vertices.data(), GetVerticesSize());
    m_indexBuffer->Update(m_indices.data(), GetIndicesSize());
}

void Mesh::Clear()
{
    m_vertices.clear();
    m_vertices.shrink_to_fit();

    m_indices.clear();
    m_indices.shrink_to_fit();
}

const std::vector<Vertex>& Mesh::GetVertices() const
{
    return m_vertices;
}

const std::vector<std::uint32_t>& Mesh::GetIndices() const
{
    return m_indices;
}

std::uint32_t Mesh::GetVertexCount() const
{
    return static_cast<std::uint32_t>(m_vertices.size());
}

std::uint32_t Mesh::GetIndexCount() const
{
    return static_cast<std::uint32_t>(m_indices.size());
}

Buffer* Mesh::GetVertexBuffer()
{
    assert(m_vertexBuffer && "Buffer not initialized.");
    return m_vertexBuffer;
}

Buffer* Mesh::GetIndexBuffer()
{
    assert(m_indexBuffer && "Buffer not initialized.");
    return m_indexBuffer;
}

VkDeviceSize Mesh::GetVerticesSize() const
{
    return m_vertices.size() * sizeof(Vertex);
}

VkDeviceSize Mesh::GetIndicesSize() const
{
    return m_indices.size() * sizeof(std::uint32_t);
}

VkDeviceSize Mesh::GetSize() const
{
    return GetVerticesSize() + GetIndicesSize();
}

std::string_view Mesh::GetName() const
{
    return m_name;
}
}
