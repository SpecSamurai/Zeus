#ifndef MODEL_TYPES
#define MODEL_TYPES

#extension GL_EXT_buffer_reference : require

struct Vertex {
    vec3 position;
    float uv_x;
    vec3 normal;
    float uv_y;
    vec4 color;
};

layout(buffer_reference, std430) readonly buffer VertexBuffer {
    Vertex vertices[];
};

layout(push_constant) uniform constants {
    mat4 transform;
    VertexBuffer vertexBuffer;
    uint materialIndex;
} entityPushConstants;

#endif // MODEL_TYPES
