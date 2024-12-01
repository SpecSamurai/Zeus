#include "GltfLoader.hpp"

#include "logging/logger.hpp"

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>

#include <filesystem>
#include <optional>

namespace Zeus
{
std::optional<bool> GltfLoader::Load(std::filesystem::path path)
{
    LOG_DEBUG("Loading GLTF asset: {}", path.string());

    auto data{ fastgltf::GltfDataBuffer::FromPath(path) };
    if (data.error() != fastgltf::Error::None)
    {
        LOG_ERROR(
            "Failed to load GLTF asset. {}",
            fastgltf::getErrorMessage(data.error()));

        return std::nullopt;
    }

    auto asset{
        m_parser.loadGltf(data.get(), path.parent_path(), PARSER_OPTIONS)
    };

    if (asset.error() != fastgltf::Error::None)
    {
        LOG_ERROR(
            "Failed to parse GLTF asset. {}",
            fastgltf::getErrorMessage(asset.error()));
        return std::nullopt;
    }

#ifndef NDEBUG
    auto validation{ fastgltf::validate(asset.get()) };
    if (validation != fastgltf::Error::None)
    {
        LOG_ERROR(
            "GLTF validation failed. {}",
            fastgltf::getErrorMessage(validation));
    }
#endif

    // call asset.get(), asset.get_if() or asset->
    for (auto& buffer : asset->buffers)
    {
        // Process the buffers.
    }

    return true;
}
}
