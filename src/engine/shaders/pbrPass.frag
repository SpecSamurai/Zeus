#version 460

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : require

#include "common.glsl"
#define FRAME_DATA_SET 1
#include "frameData.glsl"
#include "bindless.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inUV;

layout(location = 0) out vec4 outColor;

void main() {
    uint index = nonuniformEXT(materialsBuffer.parameters[0].flags);

    vec3 baseColor = texture(textures2D[index], inUV).rgb;
    vec3 emissionMap = texture(textures2D[1], inUV).rgb;
    vec3 roughnessMap = texture(textures2D[2], inUV).rgb;
    vec3 normalMap = texture(textures2D[3], inUV).rgb;

    vec3 ambient = frameBuffer.data.ambientLight.a * baseColor;

    vec3 normNormal = normalize(inNormal);
    vec3 lightDir = normalize(frameBuffer.data.directionalLightPosition - inPosition);

    float diffuseFactor = max(dot(normNormal, lightDir), 0.0);
    vec3 diffuse = diffuseFactor * frameBuffer.data.directionalLight.rgb * baseColor;

    vec3 viewDir = normalize(frameBuffer.data.viewPosition - inPosition);
    vec3 reflectDir = reflect(-lightDir, normNormal);
    float shininess = 32;
    float reflection = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = roughnessMap * reflection * frameBuffer.data.directionalLight.rgb;

    vec3 result = emissionMap + ambient + diffuse + specular;

    outColor = vec4(result, 1.0);
}
