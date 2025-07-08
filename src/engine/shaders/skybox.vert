#version 460

#extension GL_GOOGLE_include_directive : require

#include "frame_data.glsl"

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 outUVW;

void main()
{
    outUVW = inPosition;

    mat4 view = mat4(mat3(frameBuffer.data.view));
    gl_Position = frameBuffer.data.projection * view * vec4(inPosition, 1.0);
    gl_Position.z = 0.0;
}
