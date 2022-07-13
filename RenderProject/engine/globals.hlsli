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