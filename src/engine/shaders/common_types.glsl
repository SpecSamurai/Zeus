#ifndef COMMON_TYPES
#define COMMON_TYPES

struct MaterialParameters {
    float shininess;
    uint flags;
};

const uint MATERIAL_TEXTURE_BASE_COLOR_FLAG        = 1u << 0u;
const uint MATERIAL_TEXTURE_ROUGHNESS_FLAG         = 1u << 1u;
const uint MATERIAL_TEXTURE_METALNESS_FLAG         = 1u << 2u;
const uint MATERIAL_TEXTURE_DIFFUSE_COLOR_FLAG     = 1u << 3u;
const uint MATERIAL_TEXTURE_SPECULAR_FLAG          = 1u << 4u;
const uint MATERIAL_TEXTURE_GLOSSINESS_FLAG        = 1u << 5u;
const uint MATERIAL_TEXTURE_NORMAL_FLAG            = 1u << 6u;
const uint MATERIAL_TEXTURE_AMBIENT_OCCLUSION_FLAG = 1u << 7u;
const uint MATERIAL_TEXTURE_EMISSION_FLAG          = 1u << 8u;
const uint MATERIAL_TEXTURE_HEIGHT_FLAG            = 1u << 9u;

bool hasMaterialFlag(MaterialParameters materialParameters, uint flag) {
    return (materialParameters.flags & flag) != 0u;
}

struct LightParameters {
    vec3 position;
    float intensity;

    vec3 direction;
    float innerCutoff;

    vec3 ambient;
    float outerCutoff; // cutoff glm::cos(glm::radians(12.5f))

    vec3 diffuse;
    float linearFactor;

    vec3 specular;
    float quadraticFactor;

    uint flags;
};

#endif // COMMON_TYPES
