#include "camera_utils.hpp"

#include "ArcballCamera.hpp"
#include "FreeflyCamera.hpp"

#include <memory>

namespace Zeus
{
std::unique_ptr<FreeflyCamera> createFreeflyEditorCamera(float aspectRatio)
{
    return std::make_unique<FreeflyCamera>(FreeflyCamera(aspectRatio));
}

std::unique_ptr<ArcballCamera> createArcballEditorCamera(float aspectRatio)
{
    return std::make_unique<ArcballCamera>(ArcballCamera(aspectRatio));
}
}
