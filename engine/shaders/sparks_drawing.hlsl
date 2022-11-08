#include "globals.hlsli"

struct Particle
{
    float3 position;
    float spawnTime;
    float3 velocity;
    float padding;
};

RWStructuredBuffer<Particle> particlesData : register(u1);
RWBuffer<uint> particlesRange : register(u2);
static const uint BUFFER_SIZE = 128;

Texture2D<float> g_sparkColor : register(t0);

struct PS_INPUT
{
    float4 position : SV_Position;
    float2 tex_coord : TEXCOORD;
};

PS_INPUT vs_main(uint vertexID : SV_VertexID, uint instanceID : SV_InstanceID)
{
    PS_INPUT output;
    uint sparkIndex = (particlesRange[0] + instanceID) % BUFFER_SIZE;
    float3 vertex_pos = float3(0,0,0);
    
    uint vertexID_new = vertexID > 2 ? 5 - vertexID : vertexID;
    int2 xy = int2(vertexID_new == 2, vertexID_new != 0);
    vertex_pos.xy = vertexID == 4 ? float2(-0.5f, -0.5f) + xy.yx : float2(-0.5f, -0.5f) + xy;
    output.tex_coord = vertexID == 4 ? float2(1, 1) : float2(xy.x, xy.y == 0);
    
    vertex_pos *= 0.02f;
    vertex_pos = mul(float4(vertex_pos, 0), g_viewInv);
    vertex_pos += particlesData[sparkIndex].position + vertex_pos;
    output.position = mul(float4(vertex_pos, 1), g_viewProj);
    return output;
}

float4 ps_main(PS_INPUT input) : SV_Target
{
    float3 color = float3(5,3,0.1f);
    float alpha = g_sparkColor.Sample(g_samplerState, input.tex_coord);
    return float4(color, alpha);
}