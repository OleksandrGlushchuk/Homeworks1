#include "globals.hlsli"

cbuffer MeshToModel : register(b1)
{
    float4x4 g_meshToModelMatrix;
}

cbuffer LightIndex : register(b2)
{
    uint g_lightIndex;
    float3 paddingPLI;
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
struct VS_OUT
{
    float4 world_pos : POSITION;
};

VS_OUT vs_main(VS_INPUT input)
{
    VS_OUT output;
    float4x4 TransformMatrix = float4x4(input.transform_x, input.transform_y, input.transform_z, input.transform_w);

    float4 pos = mul(float4(input.position, 1.0f), g_meshToModelMatrix);
    
    pos = mul(pos, TransformMatrix);
    
    output.world_pos = pos;
    
    return output;
}

struct GS_OUT
{
    float4 position : SV_Position;
    nointerpolation uint renderTarget : SV_RenderTargetArrayIndex;
};

[maxvertexcount(3)]
void gs_main(triangle VS_OUT input[3], inout TriangleStream<GS_OUT> outputStream)
{
    [unroll]
    for (uint i = 0; i < 3; ++i)
    {
        GS_OUT output;
        output.renderTarget = g_lightIndex;
        output.position = mul(input[i].world_pos, g_viewProjDirectionalLight[g_lightIndex]);
        outputStream.Append(output);
    }
    outputStream.RestartStrip();
}