#include "globals.hlsli"

Texture2D shaderTexture : register(t0);


struct VS_INPUT
{
    float3 position : POSITION;
    float2 tex_coord : TEXCOORD0;
    //float3 normal : NORMAL;
    
    float4 mesh_to_model_x : MESH_TO_MODEL_X;
    float4 mesh_to_model_y : MESH_TO_MODEL_Y;
    float4 mesh_to_model_z : MESH_TO_MODEL_Z;
    float4 mesh_to_model_w : MESH_TO_MODEL_W;
    
    float4 transform_x : TRANSFORM_X;
    float4 transform_y : TRANSFORM_Y;
    float4 transform_z : TRANSFORM_Z;
    float4 transform_w : TRANSFORM_W;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 tex_coord : TEXCOORD0;
};

PS_INPUT vs_main(VS_INPUT input)
{
    PS_INPUT output;
    matrix MeshToModelMatrix = matrix(input.mesh_to_model_x, input.mesh_to_model_y, input.mesh_to_model_z, input.mesh_to_model_w);
    matrix TransformMatrix = matrix(input.transform_x, input.transform_y, input.transform_z, input.transform_w);
    output.position = mul(g_viewProj, mul( mul(float4(input.position, 1.0f),MeshToModelMatrix), TransformMatrix));
    output.tex_coord = input.tex_coord;
    return output;
}

float4 ps_main(PS_INPUT input) : SV_TARGET
{
    float4 textureColor;
    textureColor = shaderTexture.Sample(g_samplerState, input.tex_coord);
    return textureColor;
}