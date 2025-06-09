#ifndef COMMON
#define COMMON

#extension GL_EXT_buffer_reference : require

#define BINDLESS_SET 0
#define UNIFORM_FRAME_DATA_SET 1

#define COMBINED_IMAGE_SAMPLER_BINDING 0
#define STORAGE_BUFFER_BINDING 1

struct MaterialParameters {
    uint flags;
};

struct FrameData {
    mat4 view_projection;
};

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

layout(push_constant) uniform constants {
    mat4 model;
    VertexBuffer vertexBuffer;
    uint materialIndex;
} passPushConstants;

layout(set = BINDLESS_SET, binding = COMBINED_IMAGE_SAMPLER_BINDING) uniform sampler2D textures2D[];

layout(std430, set = BINDLESS_SET, binding = STORAGE_BUFFER_BINDING) readonly buffer MaterialsBuffer {
    MaterialParameters parameters[];
} materialsBuffer;

layout(set = UNIFORM_FRAME_DATA_SET, binding = 0) uniform UniformBufferFrame {
    FrameData data;
} frameBuffer;

#endif // COMMON
