#version 460
#extension GL_GOOGLE_include_directive : require

#include "common.glsl"

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 outColor;
layout(location = 3) out vec2 outUV;

void main()
{
    Vertex vertex = passPushConstants.vertexBuffer.vertices[gl_VertexIndex];

    vec4 worldPosition = passPushConstants.model * vec4(vertex.position, 1.0);

    outPosition = worldPosition.xyz;
    outNormal = normalize(mat3(transpose(inverse(passPushConstants.model))) * vertex.normal);
    outColor = vertex.color;
    outUV = vec2(vertex.uv_x, vertex.uv_y);

    gl_Position = frameBuffer.data.view_projection * worldPosition;
}
