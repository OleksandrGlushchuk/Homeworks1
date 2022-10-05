#ifndef _FULLSCREEN_HLSLI_
#define _FULLSCREEN_HLSLI_
#include "globals.hlsli"

struct VS_INPUT
{
    uint vertexId : SV_VertexID;
};

struct PS_INPUT
{
    float4 position : SV_Position;
    float3 dir : DIRECTION;
};

PS_INPUT fullscreenVertex(const VS_INPUT vs_input)
{
    PS_INPUT output;
    if (vs_input.vertexId == 0)
    {
        output.position = float4(-1, -1, 0, 1);
    }
    else if (vs_input.vertexId == 1)
    {
        output.position = float4(-1, 3, 0, 1);
    }
    else if (vs_input.vertexId == 2)
    {
        output.position = float4(3, -1, 0, 1);
    }
    output.dir = g_BL + (output.position.x + 1.f) * 0.5f * g_Right + (output.position.y + 1.f) * 0.5f * g_Top;
    return output;
}
#endif