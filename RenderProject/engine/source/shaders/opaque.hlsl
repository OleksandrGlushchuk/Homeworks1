#include "globals.hlsli"

Texture2D g_texture : register(t0);

cbuffer MeshToModel : register(b1)
{
    float4x4 g_meshToModelMatrix;
}

struct VS_INPUT
{
    float3 position : POSITION;
    float2 tex_coord : TEXCOORD0;
    //float3 normal : NORMAL;

    float4 transform_x : TRANSFORM_X;
    float4 transform_y : TRANSFORM_Y;
    float4 transform_z : TRANSFORM_Z;
    float4 transform_w : TRANSFORM_W;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 tex_coord : TEXCOORD0;
};

PS_INPUT vs_main(VS_INPUT input)
{
    PS_INPUT output;
    float4x4 TransformMatrix = float4x4(input.transform_x, input.transform_y, input.transform_z, input.transform_w);
    
    float4 pos = mul(g_meshToModelMatrix, float4(input.position, 1.0f));
    pos = mul(pos, TransformMatrix);
    pos = mul(g_viewProj, pos);
    
    output.position = pos;
    output.tex_coord = input.tex_coord;
    return output;
}

float4 ps_main(PS_INPUT input) : SV_TARGET
{
    float4 textureColor;
    textureColor = g_texture.Sample(g_samplerState, input.tex_coord);
    return textureColor;
}