#pragma once

#include "rhi/Image.hpp"

#include <array>
#include <cstdint>

namespace Zeus
{
enum class TextureType : std::uint8_t
{
    // Metalness-Roughness
    BASE_COLOR,
    ROUGHNESS,
    METALNESS,

    // Specular-Glossiness
    DIFFUSE_COLOR, // Albedo
    SPECULAR,
    GLOSSINESS,

    AMBIENT_OCCLUSION,
    NORMAL,
    EMISSION,
    HEIGHT,
    COUNT
};

enum class MaterialProperty : std::uint8_t
{
    ROUGHNESS,
    METALNESS,
    COUNT
};

class Material
{
public:
    using TextureIndex = std::uint32_t;
    using MaterialIndex = std::uint32_t;
    using Slot = std::uint8_t;

    Material();

    void SetTexture(
        const TextureType type,
        Image* texture,
        const Slot slot = 0);
    Image* GetTexture(const TextureType type, const Slot slot = 0);
    bool HasTexture(const TextureType type);

    void SetProperty(const MaterialProperty property, const float value);
    float GetProperty(const MaterialProperty property);

    void SetIndex(const MaterialIndex index);
    MaterialIndex GetIndex() const;

private:
    TextureIndex GetTextureIndex(const TextureType type, const Slot slot) const;

public:
    static constexpr Slot SLOTS_PER_TEXTURE_TYPE{ 4 };

    static constexpr MaterialIndex INVALID_MATERIAL_INDEX{
        std::numeric_limits<MaterialIndex>().max()
    };

    static constexpr TextureIndex INVALID_TEXTURE_INDEX{
        std::numeric_limits<TextureIndex>().max()
    };

private:
    std::array<
        Image*,
        static_cast<std::uint8_t>(TextureType::COUNT) * SLOTS_PER_TEXTURE_TYPE>
        m_textures;

    std::array<float, static_cast<std::uint8_t>(MaterialProperty::COUNT)>
        m_properties;

    MaterialIndex m_index;
};
}
