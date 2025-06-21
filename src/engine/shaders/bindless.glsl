#ifndef BINDLESS
#define BINDLESS

#define BINDLESS_SET 0

#define COMBINED_IMAGE_SAMPLER_BINDING 0
#define STORAGE_BUFFER_BINDING 1

struct MaterialParameters {
    uint flags;
    uint pad[3];
};

layout(set = BINDLESS_SET, binding = COMBINED_IMAGE_SAMPLER_BINDING) uniform sampler2D textures2D[];

layout(std430, set = BINDLESS_SET, binding = STORAGE_BUFFER_BINDING) readonly buffer MaterialsBuffer {
    MaterialParameters parameters[];
} materialsBuffer;

#endif // BINDLESS
