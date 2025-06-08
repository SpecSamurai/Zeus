#pragma once

#include "Renderer_types.hpp"
#include "rhi/Image.hpp"

#include <array>
#include <cstdint>

namespace Zeus
{
class Material
{
public:
    using Slot = std::uint8_t;

    Material();

    void SetTexture(
        const TextureType type,
        Image* texture,
        const std::uint8_t slot = 0);
    Image* GetTexture(const TextureType type, const Slot slot = 0);
    bool HasTexture(const TextureType type);

    void SetProperty(const MaterialProperty property, const float value);
    float GetProperty(const MaterialProperty property);

    void SetIndex(const Materials::Index index);
    Materials::Index GetIndex() const;

private:
    Textures::Index GetTextureIndex(const TextureType type, const Slot slot)
        const;

private:
    static constexpr Slot SLOTS_PER_TEXTURE_TYPE{ 4 };

    std::array<
        Image*,
        static_cast<std::uint32_t>(TextureType::COUNT) * SLOTS_PER_TEXTURE_TYPE>
        m_textures;

    std::array<float, static_cast<std::uint32_t>(MaterialProperty::COUNT)>
        m_properties;

    Materials::Index m_index;
};
}
