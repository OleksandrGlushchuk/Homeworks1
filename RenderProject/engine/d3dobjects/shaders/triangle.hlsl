cbuffer CameraBuffer : register(b0)
{
    matrix g_viewProj;
    float3 BL;
    float pad0;
    float3 Right;
    float pad1;
    float3 Top;
    float pad2;
};

//#include "globals.hlsl"

struct VS_INPUT
{
    float3 position : POSITION;
    float2 tex_coord : TEXCOORD0;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 tex_coord : TEXCOORD0;
};

PS_INPUT vs_main(VS_INPUT input)
{
    PS_INPUT output;
    output.position = mul(g_viewProj, float4(input.position, 1.0f));
    output.tex_coord = input.tex_coord;
    return output;
}

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);


float4 ps_main(PS_INPUT input) : SV_TARGET
{
    float4 textureColor;
    textureColor = shaderTexture.Sample(SampleType, input.tex_coord);
    return textureColor;
}