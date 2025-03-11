#version 460
#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_buffer_reference : require

#include "FrameData.glsl"

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 outColor;
layout(location = 3) out vec2 outUV;

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
    outNormal = vertex.normal; //normalize(mat3(transpose(inverse(pushConstants.model))) * vertex.normal);
    outColor = vertex.color / 2;
    outUV = vec2(vertex.uv_x, vertex.uv_y);

    gl_Position = frameData.view_projection * worldPosition;
}
