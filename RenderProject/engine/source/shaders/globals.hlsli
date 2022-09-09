#ifndef _GLOBALS_HLSLI_
#define _GLOBALS_HLSLI_
static const float M_PI = 3.14159265358979323846f; 
static const uint MAX_POINT_LIGHTS = 5;

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
    float padPFM0;
    float3 g_Right;
    float padPFM1;
    float3 g_Top;
    float padPFM2;
    float3 g_cameraPos;
    uint g_pointLightNum;
    PointLight g_pointLight[MAX_POINT_LIGHTS];
    float4x4 g_viewProjPointLight[MAX_POINT_LIGHTS][6];
};

SamplerState g_samplerState : register(s0);
SamplerState g_linearClampSampler : register(s1);
SamplerState g_pointSamplerState : register(s2);
SamplerComparisonState g_samplerComparisonState : register(s3);
#endif