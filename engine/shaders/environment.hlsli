#ifndef _ENVIRONMENT_HLSLI_
#define _ENVIRONMENT_HLSLI_
#include "globals.hlsli"
#include "pbr_helpers.hlsli"
cbuffer ReflectionMips : register(b10)
{
    uint g_reflectionMips;
    float3 paddingRM;
}
TextureCube<float3> g_irradiance : register(t10);
Texture2D<float2> g_reflectance : register(t11);
TextureCube<float3> g_reflection : register(t12);

float3 addEnvironmentDiffuseReflection(in Surface surface)
{
    return surface.albedo * (1.0f - surface.metalness) * g_irradiance.SampleLevel(g_linearClampSampler, surface.map_normal, 0.0f);
}

float3 addEnvironmentReflection(in View view, in Surface surface)
{
    float3 diffuseReflection = surface.albedo * (1.0f - surface.metalness) * g_irradiance.SampleLevel(g_linearClampSampler, surface.map_normal, 0.0f);
    float2 reflectanceLUT = g_reflectance.SampleLevel(g_linearClampSampler, float2(surface.roughness, 1.f - view.NdotV), 0);
    float3 reflectance = reflectanceLUT.x * surface.F0 + reflectanceLUT.y;
    float3 specularReflection = reflectance * g_reflection.SampleLevel(g_linearClampSampler, view.reflectionDir, surface.roughness * g_reflectionMips);
    return diffuseReflection + specularReflection;
}
#endif