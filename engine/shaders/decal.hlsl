#include "globals.hlsli"

Texture2D<float4> g_decalNormalMap : register(t0);
Texture2D<float4> g_GBufferNormals : register(t1);
Texture2D<uint> g_meshID : register(t2);
Texture2D<float> g_depth : register(t3);

static const float DECAL_ROUGHNESS = 0.1f;

struct VS_INPUT
{
    float3 position : POSITION;
    
    float3 albedo : ALBEDO;
    uint meshID : MESH_ID;
    float4 transform_x : TRANSFORM_X;
    float4 transform_y : TRANSFORM_Y;
    float4 transform_z : TRANSFORM_Z;
    float4 transform_w : TRANSFORM_W;
    
    float4 transform_inv_x : TRANSFORM_INV_X;
    float4 transform_inv_y : TRANSFORM_INV_Y;
    float4 transform_inv_z : TRANSFORM_INV_Z;
    float4 transform_inv_w : TRANSFORM_INV_W;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 decal_right : RIGHT;
    nointerpolation float3 albedo : ALBEDO;
    nointerpolation uint meshID : MESH_ID;
    nointerpolation float4x4 transform_inv : TRANSFORM_INV;
};

struct PS_OUTPUT
{
    float4 normals : SV_Target0;
    float4 albedo : SV_Target1;
    float4 rmf : SV_Target2;
    float4 emission : SV_Target3;
};

PS_INPUT vs_main(VS_INPUT input)
{
    PS_INPUT output;
    input.position *= 2;
    float4x4 WorldMatrix = float4x4(input.transform_x, input.transform_y, input.transform_z, input.transform_w);
    float4 pos = mul(float4(input.position, 1.0f), WorldMatrix);
    pos = mul(pos, g_viewProj);

    output.position = pos;
    
    float3 axisX = normalize(WorldMatrix[0].xyz);
    float3 axisY = normalize(WorldMatrix[1].xyz);
    float3 axisZ = normalize(WorldMatrix[2].xyz);
    float3x3 rotation = float3x3(axisX, axisY, axisZ);

    output.decal_right = mul(float3(1, 0, 0), rotation).xyz;
    output.albedo = input.albedo;
    output.meshID = input.meshID;
    output.transform_inv = float4x4(input.transform_inv_x, input.transform_inv_y, input.transform_inv_z, input.transform_inv_w);
    return output;
}

#include "octahedron.hlsli"

PS_OUTPUT ps_main(PS_INPUT input)
{
    PS_OUTPUT output;
    uint meshID = g_meshID.Load(int3(input.position.xy, 0));
    if (meshID != input.meshID)
    {
        discard;
        return output;
    }
    float3 sample_location = float3(input.position.xy, 0);
    float depth = g_depth.Load(sample_location);
    float4 clip_space_pos;
    clip_space_pos.x = input.position.x / (g_screenWidth / 2.f) - 1.f;
    clip_space_pos.y = -input.position.y / (g_screenHeight / 2.f) + 1.f;
    clip_space_pos.z = depth;
    clip_space_pos.w = 1.f;
    
    float4 world_pos4 = mul(clip_space_pos, g_viewProjInv);
    world_pos4 /= world_pos4.w;

    float4 pos_in_decal_space = mul(world_pos4, input.transform_inv);
    if (abs(pos_in_decal_space.x) > 1.f || abs(pos_in_decal_space.y) > 1.f || abs(pos_in_decal_space.z) > 1)
    {
        discard;
        return output;
    }
    float2 tex_coord;
    tex_coord.x = (pos_in_decal_space.x + 1) / 2;
    tex_coord.y = (pos_in_decal_space.y - 1) / (-2);
    
    float4 map_normal = g_decalNormalMap.Sample(g_samplerState, tex_coord);
    
    if (map_normal.a < 0.9f)
    {
        discard;
        return output;
    }

    float4 gbufferNormals = g_GBufferNormals.Load(int3(input.position.xy, 0));
    float3 gbufferMapNormal = unpackOctahedron(gbufferNormals.xy);
    
    map_normal.xyz = normalize(map_normal.xyz * 2 - 1);
    map_normal.y = -map_normal.y;
    
    float3 tangent = normalize(input.decal_right - gbufferMapNormal * dot(gbufferMapNormal, input.decal_right));
    
    float3 bitangent = cross(gbufferMapNormal, tangent);
    float3x3 TBN = float3x3(tangent, normalize(bitangent), gbufferMapNormal);
    map_normal.xyz = mul(map_normal.xyz, TBN);
    
   
    
    map_normal.xyz = normalize(gbufferMapNormal + map_normal.xyz);
    
    output.normals.xy = packOctahedron(map_normal.xyz);
    output.normals.zw = gbufferNormals.zw;
    output.albedo.xyz = input.albedo;
    output.emission = float4(0, 0, 0, 1);
    output.rmf = float4(DECAL_ROUGHNESS, 0, true, 1);
    return output;
}