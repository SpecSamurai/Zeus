#ifndef LIGHT_BLINN_PHONG
#define LIGHT_BLINN_PHONG

#extension GL_GOOGLE_include_directive : require

#include "common_types.glsl"

float calcAttenuation(LightParameters light, float distance) {
    return 1.0 / (1.0 + light.linearFactor * distance + light.quadraticFactor * (distance * distance));
}

float calcSpotLightConeIntensity(LightParameters light, float angleCosine) {
    float intensity = (angleCosine - light.outerCutoff) / (light.innerCutoff - light.outerCutoff);
    return clamp(intensity, 0.0, 1.0);
}

vec3 calcDirectionalLight(
    LightParameters light,
    MaterialParameters material,
    vec3 viewDirection,
    vec3 normal,
    vec3 diffuseMap,
    vec3 specularMap)
{
    // diffuse
    vec3 lightDir = normalize(-light.direction);
    float diffuseFactor = max(dot(normal, lightDir), 0.0);

    //specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularFactor = pow(max(dot(viewDirection, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * diffuseMap;
    vec3 diffuse = light.diffuse * diffuseFactor * diffuseMap;
    vec3 specular = light.specular * specularFactor * specularMap;

    return ambient + diffuse + specular;
}

vec3 calcPointLight(
    LightParameters light,
    MaterialParameters material,
    vec3 viewDirection,
    vec3 fragPosition,
    vec3 normal,
    vec3 diffuseMap,
    vec3 specularMap)
{
    // diffuse
    vec3 lightDir = normalize(light.position - fragPosition);
    float diffuseFactor = max(dot(normal, lightDir), 0.0);

    //specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularFactor = pow(max(dot(viewDirection, reflectDir), 0.0), material.shininess);

    // attenuation
    float distance = length(light.position - fragPosition);
    float attenuation = calcAttenuation(light, distance);

    vec3 ambient = light.ambient * diffuseMap;
    vec3 diffuse = light.diffuse * diffuseFactor * diffuseMap;
    vec3 specular = light.specular * specularFactor * specularMap;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

vec3 calcSpotLight(
    LightParameters light,
    MaterialParameters material,
    vec3 viewDirection,
    vec3 fragPosition,
    vec3 normal,
    vec3 diffuseMap,
    vec3 specularMap)
{
    // diffuse
    vec3 lightDir = normalize(light.position - fragPosition);
    float diffuseFactor = max(dot(normal, lightDir), 0.0);

    //specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularFactor = pow(max(dot(viewDirection, reflectDir), 0.0), material.shininess);

    // attenuation
    float distance = length(light.position - fragPosition);
    float attenuation = calcAttenuation(light, distance);

    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float intensity = calcSpotLightConeIntensity(light, theta);

    vec3 ambient = light.ambient * diffuseMap;
    vec3 diffuse = light.diffuse * diffuseFactor * diffuseMap;
    vec3 specular = light.specular * specularFactor * specularMap;

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return ambient + diffuse + specular;
}

#endif // LIGHT_BLINN_PHONG
