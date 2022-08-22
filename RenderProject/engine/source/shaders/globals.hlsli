#ifndef _GLOBALS_HLSLI_
#define _GLOBALS_HLSLI_
static const float M_PI = 3.14159265358979323846f; 
static const uint MAX_POINT_LIGHTS = 8;

struct PointLight
{
    float3 radiance;
    float padding0;
    float3 position;
    float radius;
};

cbuffer PerFrameBuffer : register(b0)
{
    float4x4 g_viewProj;
    float3 g_BL;
    float pad0;
    float3 g_Right;
    float pad1;
    float3 g_Top;
    float pad2;
    float3 g_cameraPos;
    uint g_pointLightNum;
    PointLight g_pointLight[MAX_POINT_LIGHTS];
};

SamplerState g_samplerState : register(s0);
SamplerState g_pointSamplerState : register(s1);
#endif