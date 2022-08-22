#include "globals.hlsli"

cbuffer MeshToModel : register(b1)
{
    float4x4 g_meshToModelMatrix;
}

struct VS_INPUT
{
    float3 position : POSITION;
    
    float3 emission : EMISSION;
    float4 transform_x : TRANSFORM_X;
    float4 transform_y : TRANSFORM_Y;
    float4 transform_z : TRANSFORM_Z;
    float4 transform_w : TRANSFORM_W;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 emission : EMISSION;
};

PS_INPUT vs_main(VS_INPUT input)
{
    PS_INPUT output;
    float4x4 TransformMatrix = float4x4(input.transform_x, input.transform_y, input.transform_z, input.transform_w);
    
    float4 pos = mul(float4(input.position, 1.0f), g_meshToModelMatrix);
    pos = mul(pos, TransformMatrix);
    pos = mul(pos, g_viewProj);
    
    output.position = pos;
    output.emission = input.emission;
    return output;
}

float4 ps_main(PS_INPUT input) : SV_TARGET
{
    return float4(input.emission, 1);
}