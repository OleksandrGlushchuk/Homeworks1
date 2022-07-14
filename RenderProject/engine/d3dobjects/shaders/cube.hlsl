#include "globals.hlsli"

Texture2D shaderTexture : register(t0);

cbuffer TransformMatrixBuffer : register(b1)
{
    matrix TransformMatrix;
};


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
	output.position = mul(g_viewProj, mul(TransformMatrix, float4(input.position, 1.0f)));
	output.tex_coord = input.tex_coord;
	return output;
}

float4 ps_main(PS_INPUT input) : SV_TARGET
{
	float4 textureColor;
    textureColor = shaderTexture.Sample(g_samplerState, input.tex_coord);
	return textureColor;
}