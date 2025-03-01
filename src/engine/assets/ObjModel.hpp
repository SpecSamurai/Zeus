#pragma once

#include "rendering/Mesh.hpp"

#include <memory>
#include <string_view>

namespace Zeus
{
struct ObjModel
{
    std::string_view filePath;
    std::shared_ptr<Mesh> mesh;
};
}
