#pragma once

#include "ArcballCamera.hpp"
#include "FreeflyCamera.hpp"

#include <memory>

namespace Zeus
{
std::unique_ptr<FreeflyCamera> createFreeflyEditorCamera(float aspectRatio);
std::unique_ptr<ArcballCamera> createArcballEditorCamera(float aspectRatio);
}
