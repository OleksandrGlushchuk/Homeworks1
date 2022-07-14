#include "globals.hlsli"

TextureCube tex : register(t0);

struct VS_INPUT
{
    float2 position : POSITION;
};

struct PS_INPUT
{
    float3 dir : DIRECTION;
    float4 position : SV_Position;
};

PS_INPUT vs_main(VS_INPUT input)
{
    PS_INPUT output;
    
    output.dir = g_BL + (input.position.x + 1.f) * 0.5f * g_Right + (input.position.y + 1.f) * 0.5f * g_Top;
    output.position = float4(input.position, 0, 1.0f);
    return output;
}

float4 ps_main(PS_INPUT input) : SV_Target
{
    return tex.Sample(g_samplerState, input.dir);
}