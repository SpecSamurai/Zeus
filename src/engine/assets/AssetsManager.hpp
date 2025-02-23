#pragma once

#include "gltf/GltfLoader.hpp"

#include <memory>

namespace Zeus
{
class AssetsManager
{
public:
    static std::unique_ptr<GltfLoader> GetGltfLoader()
    {
        return std::make_unique<GltfLoader>();
    }

private:
    AssetsManager() = default;
};
}
