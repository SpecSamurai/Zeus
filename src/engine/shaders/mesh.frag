#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inUV;

layout(set = 1, binding = 0) uniform sampler2D texColor;
layout(set = 1, binding = 1) uniform sampler2D texNormal;
layout(set = 1, binding = 2) uniform sampler2D texEmission;
layout(set = 1, binding = 3) uniform sampler2D texRoughness;

layout(location = 0) out vec4 outColor;

void main() {

    vec3 color = inColor.rgb * texture(texColor, inUV).rgb;
    vec3 emission = texture(texEmission, inUV).rgb;
    vec3 roughness = texture(texRoughness, inUV).rgb;
    vec3 nor = texture(texNormal, inUV).rgb;

    outColor = vec4(color + emission + roughness, 1.0);
    //outColor = vec4(nor, 1.0);
}
