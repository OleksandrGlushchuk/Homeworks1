#include "globals.hlsli"
#include "gbuffer.hlsli"
Texture2D g_colorMap : register(t0);
Texture2D<float3> g_normalMap : register(t1);
Texture2D<float> g_roughnessMap : register(t2);
Texture2D<float> g_metalnessMap : register(t3);
Texture2D<float> g_dissolubleMap : register(t4);

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
    float2 paddingMCB;
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
    float3 spherePos : SPHERE_POS;
    float sphereMaxRadius : SPHERE_MAX_RADIUS;
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
    nointerpolation float3 spherePos : SPHERE_POS;
    nointerpolation float sphereMaxRadius : SPHERE_MAX_RADIUS;
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
    output.sphereMaxRadius = input.sphereMaxRadius;
    output.spherePos = input.spherePos;
    return output;
}

#include "opaque_helpers.hlsli"

PS_OUTPUT ps_main(PS_INPUT input)
{
    float dissolutionValue = g_dissolubleMap.Sample(g_samplerState, input.tex_coord);
    float currentLifeDuration = (g_time - input.creationTime) / input.lifeTime;
    float sphereRadius = input.sphereMaxRadius * currentLifeDuration;
    float3 emission = 0;
    if (length(input.world_pos.xyz - input.spherePos) < sphereRadius)
    {
        PS_OUTPUT output;
        discard;
        return output;
    }
        
    Surface surface;
    fillSurfaceStructure(surface, input.tex_coord, input.normal, input.tangent, input.bitangent, emission, true);
    
    return GBuffer(surface, 0);
}