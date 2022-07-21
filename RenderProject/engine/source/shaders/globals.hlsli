cbuffer PerFrameBuffer : register(b0)
{
    matrix g_viewProj;
    float3 g_BL;
    float pad0;
    float3 g_Right;
    float pad1;
    float3 g_Top;
    float pad2;
    matrix g_viewProjInv;
};

SamplerState g_samplerState : register(s0);