#pragma once

#include "GltfLoader.hpp"
#include "ObjLoader.hpp"

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

    static std::unique_ptr<ObjLoader> GetObjLoader()
    {
        return std::make_unique<ObjLoader>();
    }

private:
    AssetsManager() = default;
};
}
