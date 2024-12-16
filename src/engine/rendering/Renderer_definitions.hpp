#pragma once

namespace Zeus
{
enum class RenderTargets
{
    RENDER_OUTPUT_COLOR,
    RENDER_OUTPUT_DEPTH,
    COUNT
};

enum class ShaderModuleTypes
{
    LINE_VERT,
    FLAT_COLOR_FRAG,
    COUNT
};

enum class PipelineTypes
{
    LINES,
    COUNT
};
}
