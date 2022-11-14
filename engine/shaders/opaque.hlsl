#include "globals.hlsli"
Texture2D g_colorMap : register(t0);
Texture2D<float3> g_normalMap : register(t1);
Texture2D<float> g_roughnessMap : register(t2);
Texture2D<float> g_metalnessMap : register(t3);
#include "gbuffer.hlsli"

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
    uint meshID : MESH_ID;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 tex_coord : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    nointerpolation uint meshID : MESH_ID;
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
    
    float3 axisX = normalize(WorldMatrix[0].xyz);
    float3 axisY = normalize(WorldMatrix[1].xyz);
    float3 axisZ = normalize(WorldMatrix[2].xyz);

    float3x3 rotation = float3x3(axisX, axisY, axisZ);

    output.normal = mul(input.normal, rotation).xyz;
    output.tangent = mul(input.tangent, rotation).xyz;
    output.bitangent = mul(input.bitangent, rotation).xyz;

    output.meshID = input.meshID;
   
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

#include "opaque_helpers.hlsli"

PS_OUTPUT ps_main(PS_INPUT input)
{
    Surface surface;
    fillSurfaceStructure(surface, input.tex_coord, input.normal, input.tangent, input.bitangent, float3(0,0,0), true);
    return GBuffer(surface, input.meshID);
}