#include "globals.hlsli"

TextureCube tex : register(t0);

struct VS_INPUT
{
    uint vertexId : SV_VertexID;
};

struct PS_INPUT
{
    float3 dir : DIRECTION;
    float4 position : SV_Position;
};

PS_INPUT vs_main(VS_INPUT input)
{
    PS_INPUT output;
    float2 input_position;
    if (input.vertexId == 0)
        input_position = float2(-1, -1);
    else if (input.vertexId == 1)
        input_position = float2(-1, 3);
    else if (input.vertexId == 2)
        input_position = float2(3, -1);
    
    output.dir = g_BL + (input_position.x + 1.f) * 0.5f * g_Right + (input_position.y + 1.f) * 0.5f * g_Top;
    output.position = float4(input_position, 0, 1.0f);
    return output;
}

float4 ps_main(PS_INPUT input) : SV_Target
{
    return tex.Sample(g_samplerState, input.dir);
}