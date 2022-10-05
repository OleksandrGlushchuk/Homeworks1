#include "globals.hlsli"
#include "resolve_helpers.hlsli"
#include "fullscreen.hlsli"

Texture2D<float4> g_texture : register(t0);
Texture2DMS<float4> g_textureMS : register(t1);

cbuffer PostProcessBuffer : register(b1)
{
    float g_EV100;
    float3 padding;
}

PS_INPUT vs_main(VS_INPUT input)
{
    return fullscreenVertex(input);
}

float4 ps_main(PS_INPUT input) : SV_Target
{
    float3 color;
    float3 sampleSum = 0;
    for (uint i = 0; i < g_sampleCount; ++i)
    {
        color = g_sampleCount == 1 ? g_texture.Load(int3(input.position.xy, 0)).rgb : g_textureMS.Load(input.position.xy, i).rgb;
        color = AdjustExposure(color, g_EV100);
        color = ACES(color);
        sampleSum += color;
    }
    sampleSum /= g_sampleCount;
    sampleSum = GammaCorrection(sampleSum);
    return float4(sampleSum, 1.f);
}