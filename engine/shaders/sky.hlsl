#include "globals.hlsli"
#include "fullscreen.hlsli"
TextureCube tex : register(t0);

PS_INPUT vs_main(VS_INPUT input)
{
    return fullscreenVertex(input);
}

float4 ps_main(PS_INPUT input) : SV_Target
{
    return tex.Sample(g_samplerState, input.dir);
}