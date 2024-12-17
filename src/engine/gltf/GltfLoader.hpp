#pragma once

#include "rhi/Image.hpp"

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <format>
#include <stb_image.h>

#include <cassert>
#include <filesystem>
#include <optional>
#include <string_view>

namespace Zeus
{
class GltfLoader
{
public:
    std::optional<bool> Load(std::filesystem::path path);

private:
    std::optional<Image*> loadImage(
        fastgltf::Asset& asset,
        fastgltf::Image& image);

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
