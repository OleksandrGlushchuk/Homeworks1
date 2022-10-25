#include "globals.hlsli"
#include "pbr_helpers.hlsli"
#include "shadow_helpers.hlsli"
#include "gbuffer_unpack.hlsli"

struct VS_INPUT
{
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
    float2 tex_coord : TEXCOORD;
    float padding : PADDING;
     float4x4 viewProjPointLightMatrices[6] : PL_MATRICES;
};

PS_INPUT vs_main(VS_INPUT input, uint vertexID : SV_VertexID, uint instanceID : SV_InstanceID)
{
    PS_INPUT output;
    if (vertexID == 0)
    {
        output.position = float4(-1, -1, 0, 1);
        output.tex_coord = float2(0, 1);
    }
    else if (vertexID == 1)
    {
        output.position = float4(-1, 3, 0, 1);
        output.tex_coord = float2(0, -1);
    }
    else if (vertexID == 2)
    {
        output.position = float4(3, -1, 0, 1);
        output.tex_coord = float2(2, 1);
    }
    output.pl_index = instanceID;

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
    Surface surface;
    float3 world_pos;
    
    UnpackGbuffer(input.position.xy, world_pos, surface);
    surface.map_normal = surface.isFrontFace ? surface.map_normal : -surface.map_normal;
    View view;
    fillViewStructure(view,surface.map_normal, world_pos.xyz);

    float3 transmittanceRGB = surface.emission;
    float3 pointToLight = g_pointLight[input.pl_index].position - world_pos.xyz;
    float NdotL = dot(normalize(pointToLight), surface.map_normal);
    float3 transmission = NdotL < 0 ? g_pointLight[input.pl_index].radiance * transmittanceRGB * pow(-NdotL, TRANSLUCENCY_POWER) : 0;
    transmission /= pow(max(0.001, length(pointToLight)), 2);
    
    float shadowFactor;
    float3 hdrColor = float3(0, 0, 0);
    
    shadowFactor = calcPointLightShadowFactor(world_pos.xyz, surface.map_normal, input.pl_index, g_pointLight[input.pl_index].position, input.viewProjPointLightMatrices);
    hdrColor += (1.f - shadowFactor) * 
    (CalculatePointLight(g_pointLight[input.pl_index], pointToLight, view, surface) + transmission);
    return float4(hdrColor, 1.f);
}