#ifndef _PBR_STRUCTS_HLSLI_
#define _PBR_STRUCTS_HLSLI_

struct PointLight
{
    float3 radiance;
    float padding0;
    float3 position;
    float radius;
};

struct DirectionalLight
{
    float3 radiance;
    float padding0;
    float3 direction;
    float padding1;
};

struct Surface
{
    float3 map_normal;
    float3 geometry_normal;
    float3 albedo;
    float3 F0;
    float metalness;
    float roughness;
    float3 emission;
    bool isFrontFace;
};

struct View
{
    float NdotV;
    float3 reflectionDir;
    float3 PointToCameraNormalized;
};

#endif