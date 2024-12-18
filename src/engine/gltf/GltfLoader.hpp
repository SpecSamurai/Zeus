#pragma once

#include "rendering/renderer_types.hpp"
#include "rhi/Image.hpp"

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <stb_image.h>

#include <cassert>
#include <filesystem>
#include <optional>

namespace Zeus
{
class GltfLoader
{
public:
    static GltfLoader& GetInstance();

    std::optional<std::shared_ptr<LoadedGLTF>> Load(std::filesystem::path path);

private:
    std::optional<Image*> LoadImage(
        fastgltf::Asset& asset,
        fastgltf::Image& image);

private:
    static constexpr fastgltf::Options PARSER_OPTIONS{
        fastgltf::Options::DontRequireValidAssetMember |
        fastgltf::Options::AllowDouble |
        fastgltf::Options::LoadExternalBuffers |
        fastgltf::Options::LoadExternalImages
    };

    // Optimally, you should reuse this across loads, but not across threads
    fastgltf::Parser m_parser;
};
}
