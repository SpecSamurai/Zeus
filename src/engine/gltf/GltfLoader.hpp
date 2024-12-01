#pragma once

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>

#include <filesystem>
#include <optional>

namespace Zeus
{
class GltfLoader
{
public:
    std::optional<bool> Load(std::filesystem::path path);

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
