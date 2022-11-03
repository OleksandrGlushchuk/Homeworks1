#ifndef _GLOBALS_HLSLI_
#define _GLOBALS_HLSLI_
#include "pbr_structs.hlsli"

static const float M_PI = 3.14159265358979323846f; 
static const float M_2PI = 2.f * M_PI;
static const uint MAX_POINT_LIGHTS = 5;
static const uint MAX_DIRECTIONAL_LIGHTS = 2;
static const float TRANSLUCENCY_POWER = 40;
static const float3 TRANSMITTANCE_RGB = float3(0.1f, 0.1f, 0.1f);
static const float MIN_LIGHT_RADIANCE = 0.3f;

cbuffer PerFrameBuffer : register(b0)
{
    float4x4 g_viewProj;
    float4x4 g_viewProjInv;
    float4x4 g_viewInv;
    
    float3 g_BL;
    uint g_screenWidth;
    
    float3 g_Right;
    float g_time;
    
    float3 g_Top;
    uint g_sampleCount;
    
    float3 g_cameraPos;
    uint g_screenHeight;
    
    uint g_pointLightNum;
    uint g_directionalLightNum;
    float2 padPFM1;
    
    PointLight g_pointLight[MAX_POINT_LIGHTS];
    DirectionalLight g_directionalLight[MAX_DIRECTIONAL_LIGHTS];
};

SamplerState g_samplerState : register(s0);
SamplerState g_linearClampSampler : register(s1);
SamplerState g_pointSamplerState : register(s2);
SamplerComparisonState g_samplerComparisonState : register(s3);
SamplerState g_maskedSamplerState : register(s4);

void fillViewStructure(out View view, float3 map_normal, float3 world_pos)
{
    view.PointToCameraNormalized = normalize(g_cameraPos - world_pos);
    view.NdotV = max(dot(map_normal, view.PointToCameraNormalized), 0.001f);
    view.reflectionDir = reflect(-view.PointToCameraNormalized, map_normal);
}

#endif