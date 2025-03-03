#version 460
#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_buffer_reference : require

#include "FrameData.glsl"

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 outColor;

struct Vertex {
    vec3 position;
    float uv_x;
    vec3 normal;
    float uv_y;
    vec4 color;
};

layout(buffer_reference, std430) readonly buffer VertexBuffer{
    Vertex vertices[];
};

layout( push_constant ) uniform constants
{
    mat4 model;
    VertexBuffer vertexBuffer;
} pushConstants;

void main()
{
    Vertex vertex = pushConstants.vertexBuffer.vertices[gl_VertexIndex];

    vec4 worldPosition = pushConstants.model * vec4(vertex.position, 1.0);
    outPosition = worldPosition.xyz;

    outNormal = normalize(mat3(transpose(inverse(pushConstants.model))) * vertex.normal);

    float lightValue = max(dot(outNormal, vec3(-1, 0, 0)), 0.1f);
    vec3 ambient = vertex.color.xyz *  vec3(0.1, 0, 0);

    outColor = vec4(vertex.color.xyz * lightValue * 0.6 + ambient, 1.0f);

    gl_Position = frameData.view_projection * worldPosition;
}
