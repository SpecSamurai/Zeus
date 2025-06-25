#version 460

#extension GL_GOOGLE_include_directive : require

#include "frameData.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = inColor;
    gl_Position = frameBuffer.data.view_projection * vec4(inPosition, 1.0);
}
