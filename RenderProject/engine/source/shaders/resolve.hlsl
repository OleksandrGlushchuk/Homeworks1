#include "globals.hlsli"
#include "resolve_helpers.hlsli"

Texture2D g_texture : register(t0);

cbuffer PostProcessBuffer : register(b1)
{
    float g_EV100;
    float3 padding;
}

struct VS_INPUT
{
    uint vertexId : SV_VertexID;
};

struct PS_INPUT
{
    float4 position : SV_Position;
    float2 tex_coord : TEXCOORD;
};

PS_INPUT vs_main(VS_INPUT input)
{
    PS_INPUT output;
    float2 input_position;
    float2 tex_coord;
    if (input.vertexId == 0)
    {
        input_position = float2(-1, -1);
        tex_coord = float2(0, 1);
    }
    else if (input.vertexId == 1)
    {
        input_position = float2(-1, 3);
        tex_coord = float2(0, -1);
    }
    else if (input.vertexId == 2)
    {
        input_position = float2(3, -1);
        tex_coord = float2(2, 1);
    }
    output.position = float4(input_position, 0, 1.0f);
    output.tex_coord = tex_coord;
    return output;
}

float4 ps_main(PS_INPUT input) : SV_Target
{
    float3 ldrColor = g_texture.Sample(g_pointSamplerState, input.tex_coord).rgb;
    ldrColor = AdjustExposure(ldrColor, g_EV100);
    ldrColor = ACES(ldrColor);
    ldrColor = GammaCorrection(ldrColor);
    return float4(ldrColor, 1.f);
}