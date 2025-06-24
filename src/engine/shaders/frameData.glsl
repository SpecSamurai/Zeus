#ifndef FRAME_DATA
#define FRAME_DATA

#ifndef FRAME_DATA_SET
#define FRAME_DATA_SET 0
#endif

struct FrameData {
    mat4 view_projection;
    vec3 viewPosition;
    float specularIntensity;
    vec4 ambientLight;
    vec4 directionalLight;
    vec3 directionalLightPosition;
};

layout(set = FRAME_DATA_SET, binding = 0) uniform FrameDataUniformBuffer {
    FrameData data;
} frameBuffer;

#endif // FRAME_DATA
