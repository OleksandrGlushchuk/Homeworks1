#include "globals.hlsli"
#include "pbr_helpers.hlsli"
Texture2D g_colorMap : register(t0);
Texture2D<float3> g_normalMap : register(t1);
Texture2D<float> g_roughnessMap : register(t2);
Texture2D<float> g_metalnessMap : register(t3);

cbuffer MeshToModel : register(b1)
{
    float4x4 g_meshToModelMatrix;
}

cbuffer MaterialConstantBuffer : register(b2)
{
    float g_metalnessValue;
    float g_roughnessValue;
    bool g_hasMetalnessMap;
    bool g_hasRoughnessMap;
    
    bool g_hasNormalMap;
    bool g_reversedNormalTextureY;
    float2 padding;
}

struct VS_INPUT
{
    float3 position : POSITION;
    float2 tex_coord : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;

    float4 transform_x : TRANSFORM_X;
    float4 transform_y : TRANSFORM_Y;
    float4 transform_z : TRANSFORM_Z;
    float4 transform_w : TRANSFORM_W;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 tex_coord : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float4 world_pos : WORLD_POS;
};

PS_INPUT vs_main(VS_INPUT input)
{
    PS_INPUT output;
    float4x4 TransformMatrix = float4x4(input.transform_x, input.transform_y, input.transform_z, input.transform_w);
    
    float4x4 WorldMatrix = mul(g_meshToModelMatrix, TransformMatrix);
    float4 pos = mul(float4(input.position, 1.0f), WorldMatrix);
    output.world_pos = pos;
    pos = mul(pos, g_viewProj);
    
    output.position = pos;
    output.tex_coord = input.tex_coord;
    
    output.normal = normalize(mul((float4(input.normal, 0)), WorldMatrix)).xyz;
    output.tangent = normalize(mul(float4(input.tangent, 0), WorldMatrix)).xyz;
    output.bitangent = normalize(mul(float4(input.bitangent, 0), WorldMatrix)).xyz;

    return output;
}

float4 ps_main(PS_INPUT input) : SV_TARGET
{
    float3 hdrColor = g_colorMap.Sample(g_samplerState, input.tex_coord).xyz;
    
    Material material;
    
    material.albedo = hdrColor;
    
    material.metalness = g_hasMetalnessMap ? g_metalnessMap.Sample(g_samplerState, input.tex_coord) : g_metalnessValue;
    
    material.roughness = g_hasRoughnessMap ? g_roughnessMap.Sample(g_samplerState, input.tex_coord) : g_roughnessValue;
    
    material.F0 = lerp(INSULATOR_F0, material.albedo, material.metalness);
    
    float3 map_normal;
    if(g_hasNormalMap)
    {
        map_normal = g_normalMap.Sample(g_samplerState, input.tex_coord);
        map_normal = normalize(map_normal * 2.f - 1.f);
        if (g_reversedNormalTextureY)
            map_normal.y = -map_normal.y;
        
        float3x3 TBN = float3x3(input.tangent, input.bitangent, input.normal);
        map_normal = mul(map_normal, TBN);
    }
    else
        map_normal = input.normal;
    
    hdrColor = float3(0, 0, 0);
    for (uint i = 0; i < g_pointLightNum; ++i)
    {
        hdrColor += CalculatePointLight(g_pointLight[i], g_pointLight[i].position - input.world_pos.xyz,
        normalize(g_cameraPos - input.world_pos.xyz), map_normal, input.normal, material);
    }
    return float4(hdrColor, 1.f);
}