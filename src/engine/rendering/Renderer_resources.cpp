#include "Renderer_resources.hpp"

#include "math/definitions.hpp"
#include "math/transformations.hpp"
#include "rhi/Definitions.hpp"
#include "rhi/Image.hpp"

#include <vulkan/vulkan_core.h>

#include <array>
#include <cstdint>

namespace Zeus
{
void InitializeDefaultResources()
{
    Textures::InitializeTextures();
}

void DestroyDefaultResources()
{
    Textures::Black.Destroy();
    Textures::White.Destroy();
    Textures::DebugCheckerboard.Destroy();
}

void Textures::InitializeTextures()
{
    std::uint32_t black{ toUNormInt(Math::Colors::BLACK) };
    Black = Image(
        ImageType::Texture2D,
        { 1, 1, 1 },
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        "Image Black");
    Black.Update(&black, sizeof(black), 0);

    std::uint32_t white{ toUNormInt(Math::Colors::WHITE) };
    White = Image(
        ImageType::Texture2D,
        { 1, 1, 1 },
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        "Image White");
    White.Update(&white, sizeof(white), 0);

    std::uint32_t magenta{ toUNormInt(Math::Colors::MAGENTA) };
    std::array<std::uint32_t, 16 * 16> pixels;
    for (std::uint32_t x{ 0 }; x < 16; ++x)
    {
        for (std::uint32_t y{ 0 }; y < 16; ++y)
        {
            pixels[y * 16 + x] = (x % 2) ^ (y % 2) ? magenta : black;
        }
    }

    DebugCheckerboard = Image(
        ImageType::Texture2D,
        { 16, 16, 1 },
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        "Image Debug Checkerboard");
    DebugCheckerboard.Update(pixels.data(), sizeof(pixels), 0);
}
}
