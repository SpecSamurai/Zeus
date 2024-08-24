#version 460

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 projection;
} mvp;

layout(push_constant) uniform PushConstants {
    mat4 model;
} constants;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTextureCoord;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTextureCoord;

void main() {
    gl_Position = mvp.projection * mvp.view * constants.model * vec4(inPosition, 1.0);
    outColor = inColor;
    outTextureCoord = inTextureCoord;
}