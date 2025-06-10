#version 460

#extension GL_GOOGLE_include_directive : require

#include "common.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inUV;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 color = texture(textures2D[0], inUV).rgb;
    vec3 emission = texture(textures2D[1], inUV).rgb;
    vec3 roughness = texture(textures2D[2], inUV).rgb;
    vec3 nor = texture(textures2D[3], inUV).rgb;

    outColor = vec4(color + emission + roughness, 1.0);
}
