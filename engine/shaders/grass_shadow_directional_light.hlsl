#include "globals.hlsli"

static const uint TEXTURES_IN_ONE_BUSH = 6;
static const uint MAX_VERTEX_COUNT = TEXTURES_IN_ONE_BUSH * 3;

Texture2D<float> g_opacityMap : register(t0);


struct VS_INPUT
{
    float3 grass_pos : GRASS_POS;
};

struct GS_INPUT
{
    float3 grass_pos : GRASS_POS;
    float3 vertex_pos : VERTEX_POS;
    float2 tex_coord : TEXCOORD0;
};

struct GS_OUT
{
    float4 position : SV_Position;
    nointerpolation uint slice : SV_RenderTargetArrayIndex;
    float2 tex_coord : TEXCOORD0;
};

cbuffer LightIndex : register(b2)
{
    uint g_lightIndex;
    float3 paddingPLI;
}

GS_INPUT vs_main(VS_INPUT input, uint vertexID : SV_VertexID)
{
    GS_INPUT output;
    float3 pos;
    pos.z = 0;
    if (vertexID == 0 || vertexID == 5)
    {
        pos.xy = float2(-0.5f, -0.5f);
        output.tex_coord = float2(0, 1);
    }
    else if (vertexID == 1)
    {
        pos.xy = float2(-0.5f, 0.5f);
        output.tex_coord = float2(0, 0);
    }
    else if (vertexID == 2 || vertexID == 3)
    {
        pos.xy = float2(0.5f, 0.5f);
        output.tex_coord = float2(1, 0);
    }
    else if (vertexID == 4)
    {
        pos.xy = float2(0.5f, -0.5f);
        output.tex_coord = float2(1, 1);
    }

    output.vertex_pos = pos;
    output.grass_pos = input.grass_pos;
    return output;
}

[maxvertexcount(MAX_VERTEX_COUNT)]
void gs_main(triangle GS_INPUT input[3], inout TriangleStream<GS_OUT> outputStream)
{
    float angle = 2.f * M_PI / TEXTURES_IN_ONE_BUSH;
    float angle_cos, angle_sin;

    [unroll]
    for (uint i = 0; i < TEXTURES_IN_ONE_BUSH; ++i)
    {
        sincos(angle * i, angle_sin, angle_cos);
        float3x3 rotate_y = float3x3(float3(angle_cos, 0, angle_sin), float3(0, 1, 0), float3(-angle_sin, 0, angle_cos));
            
        [unroll]
        for (uint vertex = 0; vertex < 3; ++vertex)
        {
            GS_OUT output;
            
            float3 pos = input[vertex].vertex_pos;
            pos = mul(pos, rotate_y);
            pos += input[vertex].grass_pos;

            output.slice = g_lightIndex;
            output.position = mul(float4(pos, 1.f), g_viewProjDirectionalLight[g_lightIndex]);
            output.tex_coord = input[vertex].tex_coord;
            
            outputStream.Append(output);
        }
        outputStream.RestartStrip();
    }
}

float4 ps_main(GS_OUT input) : SV_Target
{
    float alpha = g_opacityMap.Sample(g_samplerState, input.tex_coord);
    return float4(0, 0, 0, alpha);
}
