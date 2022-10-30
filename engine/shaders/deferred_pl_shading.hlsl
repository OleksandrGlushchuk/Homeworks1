#include "globals.hlsli"
#include "pbr_helpers.hlsli"
#include "shadow_helpers.hlsli"
#include "gbuffer_unpack.hlsli"

struct VS_INPUT
{
    float3 vertex_pos : VERTEX_POS;
    
    uint pl_index : PL_INDEX;
    float4 viewProj_rx : VIEWPROJ_RX;
    float4 viewProj_ry : VIEWPROJ_RY;
    float4 viewProj_rz : VIEWPROJ_RZ;
    float4 viewProj_rw : VIEWPROJ_RW;
    
    float4 viewProj_lx : VIEWPROJ_LX;
    float4 viewProj_ly : VIEWPROJ_LY;
    float4 viewProj_lz : VIEWPROJ_LZ;
    float4 viewProj_lw : VIEWPROJ_LW;
    
    float4 viewProj_ux : VIEWPROJ_UX;
    float4 viewProj_uy : VIEWPROJ_UY;
    float4 viewProj_uz : VIEWPROJ_UZ;
    float4 viewProj_uw : VIEWPROJ_UW;
    
    float4 viewProj_dx : VIEWPROJ_DX;
    float4 viewProj_dy : VIEWPROJ_DY;
    float4 viewProj_dz : VIEWPROJ_DZ;
    float4 viewProj_dw : VIEWPROJ_DW;
    
    float4 viewProj_fx : VIEWPROJ_FX;
    float4 viewProj_fy : VIEWPROJ_FY;
    float4 viewProj_fz : VIEWPROJ_FZ;
    float4 viewProj_fw : VIEWPROJ_FW;
    
    float4 viewProj_bx : VIEWPROJ_BX;
    float4 viewProj_by : VIEWPROJ_BY;
    float4 viewProj_bz : VIEWPROJ_BZ;
    float4 viewProj_bw : VIEWPROJ_BW;
};

struct PS_INPUT
{
    float4 position : SV_Position;
    nointerpolation uint pl_index : PL_INDEX;
    float3 world_pos : WORLD_POS;
    nointerpolation float4x4 viewProjPointLightMatrices[6] : PL_MATRICES;
};

PS_INPUT vs_main(VS_INPUT input)
{
    PS_INPUT output;
    
    float maxLightPart = max(g_pointLight[input.pl_index].radiance.r,
    max(g_pointLight[input.pl_index].radiance.g, g_pointLight[input.pl_index].radiance.b));
    
    float sphereRadius = sqrt(maxLightPart / MIN_LIGHT_RADIANCE);
    
    output.position = float4(input.vertex_pos * sphereRadius + g_pointLight[input.pl_index].position, 1);
    output.world_pos = output.position.xyz;
    output.position = mul(output.position, g_viewProj);
    
    output.pl_index = input.pl_index;

    output.viewProjPointLightMatrices[0] = float4x4(input.viewProj_rx, input.viewProj_ry, input.viewProj_rz, input.viewProj_rw);
    output.viewProjPointLightMatrices[1] = float4x4(input.viewProj_lx, input.viewProj_ly, input.viewProj_lz, input.viewProj_lw);
    output.viewProjPointLightMatrices[2] = float4x4(input.viewProj_ux, input.viewProj_uy, input.viewProj_uz, input.viewProj_uw);
    output.viewProjPointLightMatrices[3] = float4x4(input.viewProj_dx, input.viewProj_dy, input.viewProj_dz, input.viewProj_dw);
    output.viewProjPointLightMatrices[4] = float4x4(input.viewProj_fx, input.viewProj_fy, input.viewProj_fz, input.viewProj_fw);
    output.viewProjPointLightMatrices[5] = float4x4(input.viewProj_bx, input.viewProj_by, input.viewProj_bz, input.viewProj_bw);
    return output;
}

float4 ps_main(PS_INPUT input) : SV_Target
{
    if (g_depth.Load(int3(input.position.xy, 0)) < input.position.z)
    {
        return 0;
    }
    
    Surface surface;
    float3 world_pos;
    UnpackGbuffer(input.position.xy, world_pos, surface);
    
    View view;
    fillViewStructure(view, surface.map_normal, world_pos.xyz);
    
    float shadowFactor;
    float3 hdrColor = float3(0, 0, 0);
    
    shadowFactor = calcPointLightShadowFactor(world_pos.xyz, surface.map_normal, input.pl_index, g_pointLight[input.pl_index].position, input.viewProjPointLightMatrices);
    hdrColor += (1.f - shadowFactor) * CalculatePointLight(g_pointLight[input.pl_index], g_pointLight[input.pl_index].position - world_pos.xyz, view, surface);
    return float4(hdrColor, 1.f);
    //return float4(0, 1, 0, 1);
}