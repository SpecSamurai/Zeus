#pragma once

#include "rendering/Renderer_types.hpp"
#include "rhi/Image.hpp"

#include <array>
#include <cstdint>

namespace Zeus
{
class Material
{
public:
    Material();

    void SetTexture(
        const TextureType type,
        Image* texture,
        const std::uint8_t slot = 0);
    Image* GetTexture(const TextureType type, const std::uint8_t slot = 0);
    bool HasTexture(const TextureType type);

    void SetProperty(const MaterialProperty property, const float value);
    float GetProperty(const MaterialProperty property);

    void SetIndex(const std::uint32_t index);
    std::uint32_t GetIndex() const;

private:
    std::uint32_t GetTextureIndex(
        const TextureType type,
        const std::uint8_t slot) const;

private:
    static constexpr std::uint8_t SLOTS_PER_TEXTURE_TYPE{ 4 };

    std::array<
        Image*,
        static_cast<std::uint32_t>(TextureType::COUNT) * SLOTS_PER_TEXTURE_TYPE>
        m_textures;

    std::array<float, static_cast<std::uint32_t>(MaterialProperty::COUNT)>
        m_properties;

    // BINDLESS
    std::uint32_t m_index;
};
}
