#include "globals.hlsli"

cbuffer MeshToModel : register(b1)
{
    float4x4 g_meshToModelMatrix;
}

cbuffer DirectionalLightShadowBuffer : register(b2)
{
    uint g_lightIndex;
    float3 paddingDLSB;
    float4x4 g_viewProjDirectionalLight;
}

struct VS_INPUT
{
    float3 position : POSITION;
    float2 tex_coord : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;

    float4 transform_x : TRANSFORM_X;
    float4 transform_y : TRANSFORM_Y;
    float4 transform_z : TRANSFORM_Z;
    float4 transform_w : TRANSFORM_W;
    float3 spherePos : SPHERE_POS;
    float sphereVelocity : SPHERE_VELOCITY;
    float creationTime : CREATION_TIME;
};
struct VS_OUT
{
    float4 world_pos : WORLD_POS;
    float2 tex_coord : TEXCOORD0;
    nointerpolation float sphereVelocity : SPHERE_VELOCITY;
    nointerpolation float creationTime : CREATION_TIME;
    nointerpolation float3 spherePos : SPHERE_POS;
};

VS_OUT vs_main(VS_INPUT input)
{
    VS_OUT output;
    float4x4 TransformMatrix = float4x4(input.transform_x, input.transform_y, input.transform_z, input.transform_w);

    float4 pos = mul(float4(input.position, 1.0f), g_meshToModelMatrix);
    
    pos = mul(pos, TransformMatrix);
    
    output.world_pos = pos;
    output.tex_coord = input.tex_coord;
    output.sphereVelocity = input.sphereVelocity;
    output.spherePos = input.spherePos;
    output.creationTime = input.creationTime;
    
    return output;
}

struct GS_OUT
{
    float4 position : SV_Position;
    nointerpolation uint renderTarget : SV_RenderTargetArrayIndex;
    float2 tex_coord : TEXCOORD0;
    nointerpolation float sphereVelocity : SPHERE_VELOCITY;
    nointerpolation float creationTime : CREATION_TIME;
    
    float3 world_pos : WORLD_POS;
    nointerpolation float3 spherePos : SPHERE_POS;
};

[maxvertexcount(3)]
void gs_main(triangle VS_OUT input[3], inout TriangleStream<GS_OUT> outputStream)
{
    [unroll]
    for (uint i = 0; i < 3; ++i)
    {
        GS_OUT output;
        output.renderTarget = g_lightIndex;
        output.position = mul(input[i].world_pos, g_viewProjDirectionalLight);
        output.tex_coord = input[i].tex_coord;
        output.sphereVelocity = input[i].sphereVelocity;
        output.spherePos = input[i].spherePos;
        output.creationTime = input[i].creationTime;
        output.world_pos = input[i].world_pos.xyz;
        outputStream.Append(output);
    }
    outputStream.RestartStrip();
}

Texture2D<float> g_dissolubleMap : register(t4);

float4 ps_main(GS_OUT input) : SV_Target
{
    float currentLifeDuration = (g_time - input.creationTime);
    float sphereRadius = input.sphereVelocity * (currentLifeDuration + g_deltaTime - FRAME_DURATION);
    float l = length(input.world_pos.xyz - input.spherePos);
    float alpha = l < sphereRadius ? 0 : 1;

    return float4(0, 0, 0, alpha);
}