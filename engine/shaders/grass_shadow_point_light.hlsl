#include "globals.hlsli"
#include "wind.hlsli"

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

cbuffer PointLightShadowBuffer : register(b2)
{
    uint g_lightIndex;
    float3 paddingPLSB;
    float4x4 g_viewProjPointLight[6];
}

GS_INPUT vs_main(VS_INPUT input, uint vertexID : SV_VertexID)
{
    GS_INPUT output;
    float3 pos;
    pos.z = 0;
    
    uint vertexID_new = vertexID > 2 ? 5 - vertexID : vertexID;
    int2 xy = int2(vertexID_new == 2, vertexID_new != 0);
    pos.xy = vertexID == 4 ? float2(-0.5f, -0.5f) + xy.yx : float2(-0.5f, -0.5f) + xy;
    output.tex_coord = vertexID == 4 ? float2(1, 1) : float2(xy.x, xy.y == 0);

    output.vertex_pos = pos;
    output.grass_pos = input.grass_pos;
    return output;
}

[maxvertexcount(MAX_VERTEX_COUNT * 6)]
void gs_main(triangle GS_INPUT input[3], inout TriangleStream<GS_OUT> outputStream)
{
    float angle = M_PI / TEXTURES_IN_ONE_BUSH;
    float angle_cos, angle_sin;
    
    float2 inst_pos = input[0].grass_pos.xz;
    float sw, cw;
    sw = WIND_DIR.y;
    cw = WIND_DIR.x;
    
    float3x3 windMatr = float3x3(float3(cw, 0, -sw), float3(0, 1, 0), float3(sw, 0, cw));
    float3x3 windInvMatr = transpose(windMatr);
    float wangle = computeGrassAngle(inst_pos, float2(1, 0));
    wangle = abs(wangle) > 0.01f ? wangle : sign(wangle) * 0.01f;
    float R = 1.f / wangle;

    sincos(wangle, sw, cw);
    float3x3 windRotationMatrix = float3x3(float3(cw, -sw, 0), float3(sw, cw, 0), float3(0, 0, 1));
    
    [unroll]
    for (uint face = 0; face < 6; ++face)
    {
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

                if (pos.y > 0)
                {
                    pos = mul(pos, windMatr);
                
                    pos.x -= R;
                    pos = mul(pos, windRotationMatrix);
                
                    pos.x += R;
                    pos = mul(pos, windInvMatr);
                }
                pos += input[vertex].grass_pos;
                output.slice = g_lightIndex * 6 + face;
                output.position = mul(float4(pos, 1.f), g_viewProjPointLight[face]);
                output.tex_coord = input[vertex].tex_coord;
            
                outputStream.Append(output);
            }
            outputStream.RestartStrip();
        }
    }
}

float4 ps_main(GS_OUT input) : SV_Target
{
    float alpha = g_opacityMap.Sample(g_samplerState, input.tex_coord);
    return float4(0,0,0,alpha);
}
