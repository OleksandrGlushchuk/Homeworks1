TextureCube tex : register(t0);
SamplerState samplerState : register(s0);

cbuffer CameraBuffer : register(b0)
{
    matrix g_viewProj;
    float3 BL;
    float pad0;
    float3 Right;
    float pad1;
    float3 Top;
    float pad2;
};

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
    
    output.dir = BL + (input.position.x + 1.f) * 0.5f * Right + (input.position.y + 1.f) * 0.5f * Top;
    output.position = float4(input.position, 1.0f, 1.0);
    return output;
}

float4 ps_main(PS_INPUT input) : SV_Target
{
    return tex.Sample(samplerState, input.dir);
}