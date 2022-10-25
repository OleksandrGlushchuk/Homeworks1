#ifndef _GBUFFER_HLSLI_
#define _GBUFFER_HLSLI_
#include "pbr_structs.hlsli"
#include "octahedron.hlsli"

struct PS_OUTPUT
{
    float4 normals : SV_Target0;
    float4 albedo : SV_Target1;
    float4 rmf : SV_Target2;
    float4 emission : SV_Target3;
    uint modelID : SV_Target4;
};

PS_OUTPUT GBuffer(in Surface surface, in uint modelID)
{
    PS_OUTPUT ps_output;
    ps_output.normals = float4(packOctahedron(surface.map_normal.xyz), packOctahedron(surface.geometry_normal.xyz));
    ps_output.albedo.xyz = surface.albedo;
    ps_output.rmf = float4(surface.roughness, surface.metalness, surface.isFrontFace, 1.f);
    ps_output.emission = float4(surface.emission, 1.f);
    ps_output.modelID = modelID;
    return ps_output;
}

#endif