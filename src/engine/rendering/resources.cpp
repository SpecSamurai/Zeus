#include "resources.hpp"

#include "core/defines.hpp"

#include <vulkan/vulkan_core.h>

#include <stb_image.h>

namespace Zeus
{
void InitializeDefaultResources()
{
    Textures::InitializeTextures();
    Samplers::InitializeSamplers();
}

void DestroyDefaultResources()
{
    Textures::DebugCheckerboard.Destroy();

    Samplers::DefaultNearest.Destroy();
    Samplers::DefaultLinear.Destroy();
}

void Textures::InitializeTextures()
{
    DebugCheckerboard =
        Image::LoadFromFile(AssetsFolderPath + "/debug_checkerboard.png");
}

void Samplers::InitializeSamplers()
{
    DefaultNearest = Sampler(VK_FILTER_NEAREST, VK_FILTER_NEAREST);
    DefaultLinear = Sampler(VK_FILTER_LINEAR, VK_FILTER_LINEAR);
}
}
