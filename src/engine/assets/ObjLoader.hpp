#pragma once

#include "ObjModel.hpp"

#include <optional>
#include <string_view>

namespace Zeus
{
class ObjLoader
{
public:
    [[nodiscard]] std::optional<ObjModel> Load(std::string_view filePath);
};
}
