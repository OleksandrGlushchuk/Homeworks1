static const float M_PI = 3.14159265358979323846f; 
static const float HALF_SPHERE_SOLID_ANGLE = 2.f * M_PI;
static const uint MAX_LIGHTS = 8;

struct PointLightBuffer
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
    PointLightBuffer g_pointLightBuffer[MAX_LIGHTS];
};

SamplerState g_samplerState : register(s0);