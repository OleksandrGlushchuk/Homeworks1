#include "globals.hlsli"
#include "pbr_helpers.hlsli"
#include "environment.hlsli"

cbuffer MeshToModel : register(b1)
{
    float4x4 g_meshToModelMatrix;
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
    float creationTime : CREATION_TIME;
    float lifeTime : LIFE_TIME;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 tex_coord : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float4 world_pos : WORLD_POS;
    nointerpolation float creationTime : CREATION_TIME;
    nointerpolation float lifeTime : LIFE_TIME;
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

    output.normal = mul((float4(input.normal, 0)), WorldMatrix).xyz;
    output.tangent = mul(float4(input.tangent, 0), WorldMatrix).xyz;
    output.bitangent = mul(float4(input.bitangent, 0), WorldMatrix).xyz;
    
    output.creationTime = input.creationTime;
    output.lifeTime = input.lifeTime;
   
    return output;
}

cbuffer MaterialConstantBuffer : register(b2)
{
    float g_metalnessValue;
    float g_roughnessValue;
    bool g_hasMetalnessMap;
    bool g_hasRoughnessMap;
    
    bool g_hasNormalMap;
    bool g_reversedNormalTextureY;
    float2 paddingMCB;
}

cbuffer PointLightDSResolutionBuffer : register(b3)
{
    float g_pointLightDSResolution;
    float3 paddingPLDSRB;
}

Texture2D g_colorMap : register(t0);
Texture2D<float3> g_normalMap : register(t1);
Texture2D<float> g_roughnessMap : register(t2);
Texture2D<float> g_metalnessMap : register(t3);
#include "opaque_helpers.hlsli"

Texture2D<float> g_dissolubleMap : register(t4);

float4 ps_main(PS_INPUT input) : SV_TARGET
{
    float dissolutionValue = g_dissolubleMap.Sample(g_samplerState, input.tex_coord);
    float currentLifeDuration = (g_timeSinceEpoch - input.creationTime) / input.lifeTime;
    if (currentLifeDuration < dissolutionValue)
    {
        //discard;
        return float4(0.1f, 0.5f, 1.f, 0.3f);
    }
    else if (abs(currentLifeDuration - dissolutionValue) <= 0.05f)
    {
        
        return float4(0.1f, 0.5f, 10.f, 0.7f);
    }
        
    
    
    Surface surface;
    View view;
    fillSurfaceStructure(surface, input.tex_coord, input.normal, input.tangent, input.bitangent);
    fillViewStructure(view, surface.map_normal, input.world_pos.xyz);

    float3 hdrColor = float3(0, 0, 0);
    for (uint i = 0; i < g_pointLightNum; ++i)
    {
        hdrColor += CalculatePointLight(g_pointLight[i], g_pointLight[i].position - input.world_pos.xyz, view, surface);
    }
    
    for (uint j = 0; j < g_directionalLightNum; ++j)
    {
        hdrColor += CalculateDirectionalLight(g_directionalLight[j], view, surface);
    }
    hdrColor += addEnvironmentReflection(view, surface);
    
    float4 result = float4(hdrColor, 1.f);
    return result;
}