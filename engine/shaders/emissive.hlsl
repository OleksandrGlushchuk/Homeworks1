#include "globals.hlsli"

cbuffer MeshToModel : register(b1)
{
    float4x4 g_meshToModelMatrix;
}

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    
    float3 emission : EMISSION;
    float padding : PADDING;
    float4 transform_x : TRANSFORM_X;
    float4 transform_y : TRANSFORM_Y;
    float4 transform_z : TRANSFORM_Z;
    float4 transform_w : TRANSFORM_W;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 worldPos : WORLS_POS;
    float3 normal : NORMAL;
    nointerpolation float3 emission : EMISSION;
};

PS_INPUT vs_main(VS_INPUT input)
{
    PS_INPUT output;
    float4x4 TransformMatrix = float4x4(input.transform_x, input.transform_y, input.transform_z, input.transform_w);
    
    float4x4 WorldMatrix = mul(g_meshToModelMatrix, TransformMatrix);
    
    float4 pos = mul(float4(input.position, 1.0f), WorldMatrix);
    
    output.worldPos = pos.xyz;
    output.normal = mul(float4(input.normal, 0.f), WorldMatrix).xyz;
    pos = mul(pos, g_viewProj);
    
    output.position = pos;
    output.emission = input.emission;
    return output;
}

float4 ps_main(PS_INPUT input) : SV_TARGET
{
    float3 normal = normalize(input.normal);
    float3 cameraDir = normalize(g_cameraPos - input.worldPos);

    float3 normedEmission = input.emission / max(input.emission.x, max(input.emission.y, max(input.emission.z, 1.0)));

    float NoV = dot(cameraDir, normal);
    return float4(lerp(normedEmission * 0.33, input.emission, pow(max(0.0, NoV), 8)), 1.f);
}