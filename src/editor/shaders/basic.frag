#version 460

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inTextureCoord;

layout(binding = 1) uniform sampler2D textureSampler;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(textureSampler, inTextureCoord);
    //outColor = vec4(inColor * texture(textureSampler, inTextureCoord).rgb, 1.0);
    //outColor = vec4(inColor, 1.0);
}
