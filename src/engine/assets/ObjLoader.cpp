#include "ObjLoader.hpp"

#include "ObjModel.hpp"
#include "logging/logger.hpp"
#include "math/definitions.hpp"
#include "rendering/Mesh.hpp"
#include "rhi/Vertex.hpp"

#include <format>
#include <memory>
#include <tiny_obj_loader.h>

#include <optional>
#include <string_view>
#include <vector>

namespace Zeus
{
[[nodiscard]] std::optional<ObjModel> ObjLoader::Load(std::string_view filePath)
{
    tinyobj::ObjReader objReader;

    if (!objReader.ParseFromFile(filePath.data()) || !objReader.Valid())
    {
        LOG_ERROR(
            "Failed to parse Obj file {}. {}",
            filePath,
            objReader.Error());

        return std::nullopt;
    }

    if (!objReader.Warning().empty())
    {
        LOG_WARNING(
            "Parsed Obj file {} with issues. {}",
            filePath,
            objReader.Warning());
    }

    std::shared_ptr<Mesh> mesh{ std::make_shared<Mesh>() };
    const auto& attributes{ objReader.GetAttrib() };

    for (const auto& shape : objReader.GetShapes())
    {
        mesh->name = shape.name;
        LOG_DEBUG("Parsing shape: {}", shape.name);

        for (const auto& index : shape.mesh.indices)
        {
            Vertex vertex;

            std::size_t vertexIndex{ static_cast<std::size_t>(
                index.vertex_index) };

            vertex.position = Math::Vector3f(
                attributes.vertices[3 * vertexIndex],
                attributes.vertices[3 * vertexIndex + 1],
                attributes.vertices[3 * vertexIndex + 2]);

            vertex.color = Math::Color(
                attributes.colors[3 * vertexIndex],
                attributes.colors[3 * vertexIndex + 1],
                attributes.colors[3 * vertexIndex + 2],
                1.f);

            if (index.normal_index != -1)
            {
                std::size_t normalIndex{ static_cast<std::size_t>(
                    index.normal_index) };

                vertex.normal = Math::Vector3f(
                    attributes.normals[3 * normalIndex],
                    attributes.normals[3 * normalIndex + 1],
                    attributes.normals[3 * normalIndex + 2]);
            }

            if (index.texcoord_index != -1)
            {
                std::size_t texcoordIndex{ static_cast<std::size_t>(
                    index.texcoord_index) };

                vertex.uvX = attributes.texcoords[2 * texcoordIndex];
                vertex.uvY = attributes.texcoords[2 * texcoordIndex + 1];
                // 1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            }

            mesh->m_indices.emplace_back(mesh->m_vertices.size());
            mesh->m_vertices.emplace_back(vertex);
        }
    }

    mesh->m_vertexBuffer = Buffer(
        std::format("Buffer_Vertex_{}", mesh->name),
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        mesh->m_vertices.size() * sizeof(Vertex),
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        false);

    mesh->m_indexBuffer = Buffer(
        std::format("Buffer_Index_{}", mesh->name),
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        mesh->m_indices.size() * sizeof(std::uint32_t),
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        false);

    mesh->m_vertexBuffer.Update(
        mesh->m_vertices.data(),
        mesh->m_vertices.size() * sizeof(Vertex));

    mesh->m_indexBuffer.Update(
        mesh->m_indices.data(),
        mesh->m_indices.size() * sizeof(std::uint32_t));

    return ObjModel{
        .filePath = filePath,
        .mesh = std::move(mesh),
    };
}
}
