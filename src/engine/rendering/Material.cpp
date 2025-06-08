#include "Material.hpp"
#include "rendering/Renderer_types.hpp"

#include <cassert>
#include <cstdint>

namespace Zeus
{
Material::Material()
    : m_textures{ nullptr },
      m_properties{ 0.f },
      m_index{ Materials::InvalidIndex }
{
}

void Material::SetTexture(
    const TextureType type,
    Image* texture,
    const std::uint8_t slot)
{
    Textures::Index index{ GetTextureIndex(type, slot) };

    m_textures[index] = texture;
}

Image* Material::GetTexture(const TextureType type, const std::uint8_t slot)
{
    Textures::Index index{ GetTextureIndex(type, slot) };

    return m_textures[index];
}

bool Material::HasTexture(const TextureType type)
{
    for (std::uint8_t slot{ 0 }; slot < SLOTS_PER_TEXTURE_TYPE; ++slot)
    {
        if (GetTexture(type, slot) != nullptr)
            return true;
    }

    return false;
}

void Material::SetProperty(const MaterialProperty property, const float value)
{
    m_properties[static_cast<std::uint8_t>(property)] = value;
}

float Material::GetProperty(const MaterialProperty property)
{
    return m_properties[static_cast<std::uint8_t>(property)];
}

void Material::SetIndex(const Materials::Index index)
{
    m_index = index;
}

Materials::Index Material::GetIndex() const
{
    return m_index;
}

Textures::Index Material::GetTextureIndex(
    const TextureType type,
    const Slot slot) const
{
    assert(slot < SLOTS_PER_TEXTURE_TYPE && "Invalid slot");
    return (static_cast<std::uint32_t>(type) * SLOTS_PER_TEXTURE_TYPE) + slot;
}
}
