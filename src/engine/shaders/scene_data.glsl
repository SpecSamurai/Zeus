layout(set = 0, binding = 0) uniform SceneData
{
	mat4 view;
	mat4 projection;
	mat4 viewProjection;

	vec4 ambientColor;

	vec3 directionalLightDir;
	float directionalLightFactor;
	vec4 directionalLightColor;
} sceneData;
