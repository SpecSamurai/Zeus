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
        const TextureTypes type,
        Image* texture,
        const std::uint8_t slot = 0);
    Image* GetTexture(const TextureTypes type, const std::uint8_t slot = 0);
    bool HasTexture(const TextureTypes type);

    void SetProperty(const MaterialProperties property, const float value);
    float GetProperty(const MaterialProperties property);

    void SetIndex(const std::uint32_t index);
    std::uint32_t GetIndex() const;

private:
    std::uint32_t GetTextureIndex(
        const TextureTypes type,
        const std::uint8_t slot) const;

private:
    static constexpr std::uint8_t SLOTS_PER_TEXTURE_TYPE{ 4 };

    std::array<
        Image*,
        static_cast<std::uint32_t>(TextureTypes::COUNT) *
            SLOTS_PER_TEXTURE_TYPE>
        m_textures;

    std::array<float, static_cast<std::uint32_t>(MaterialProperties::COUNT)>
        m_properties;

    // BINDLESS
    std::uint32_t m_index;
};
}
