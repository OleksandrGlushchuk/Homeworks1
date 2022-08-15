#include "globals.hlsli"

Texture2D g_texture : register(t0);

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

inline float3 AdjustExposure(uniform float3 radiance, uniform float _EV100)
{
    float LMax = (78.0f / (0.65f * 100.0f)) * pow(2.0f, _EV100);
    return radiance / LMax;
}

inline float3 ACES(uniform float3 hdr)
{
    float3x3 m1 = float3x3(float3(0.59719f, 0.07600f, 0.02840f), float3(0.35458f, 0.90834f, 0.13383f), float3(0.04823f, 0.01566f, 0.83777f));
    float3x3 m2 = float3x3(float3(1.60475f, -0.10208, -0.00327f), float3(-0.53108f, 1.10813, -0.07276f), float3(-0.07367f, -0.00605, 1.07602f));
    float3 v = mul(hdr, m1);

    float3 a = v * (v + float3(0.0245786f, 0.0245786f, 0.0245786f)) - float3(0.000090537f, 0.000090537f, 0.000090537f);
    float3 b = v * (0.983729f * v + float3(0.4329510f, 0.4329510f, 0.4329510f)) + float3(0.238081f, 0.238081f, 0.238081f);

    float3 ldr = mul(a / b, m2);
    
    ldr[0] = clamp(ldr[0], 0.0f, 1.0f);
    ldr[1] = clamp(ldr[1], 0.0f, 1.0f);
    ldr[2] = clamp(ldr[2], 0.0f, 1.0f);
    return ldr;
}

inline float3 GammaCorrection(uniform float3 radiance)
{
    return float3(
			pow(radiance[0], 1.f / 2.2f),
			pow(radiance[1], 1.f / 2.2f),
			pow(radiance[2], 1.f / 2.2f)
		);
}

cbuffer PostProcessBuffer : register(b1)
{
    float g_EV100;
    float3 padding;
}

float4 ps_main(PS_INPUT input) : SV_Target
{
    float3 ldrColor = g_texture.Sample(g_samplerState, input.tex_coord).rgb;
    ldrColor = AdjustExposure(ldrColor, g_EV100);
    ldrColor = ACES(ldrColor);
    ldrColor = GammaCorrection(ldrColor);
    return float4(ldrColor, 1.f);
}