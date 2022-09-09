#ifndef _OPAQUE_HELPERS_HLSLI_
#define _OPAQUE_HELPERS_HLSLI_

void fillSurfaceStructure(out Surface surface, float2 tex_coord, float3 normal, float3 tangent, float3 bitangent)
{
    surface.albedo = g_colorMap.Sample(g_samplerState, tex_coord).xyz;
    surface.metalness = g_hasMetalnessMap ? g_metalnessMap.Sample(g_samplerState, tex_coord) : g_metalnessValue;
    surface.roughness = g_hasRoughnessMap ? g_roughnessMap.Sample(g_samplerState, tex_coord) : g_roughnessValue;
    surface.F0 = lerp(INSULATOR_F0, surface.albedo, surface.metalness);
    surface.geometry_normal = normalize(normal);

    if (g_hasNormalMap)
    {
        surface.map_normal = g_normalMap.Sample(g_samplerState, tex_coord);
        surface.map_normal = normalize(surface.map_normal * 2.f - 1.f);
        surface.map_normal.y = g_reversedNormalTextureY ? -surface.map_normal.y : surface.map_normal.y;
        float3x3 TBN = float3x3(normalize(tangent), normalize(bitangent), surface.geometry_normal);
        surface.map_normal = mul(surface.map_normal, TBN);
    }
    else
        surface.map_normal = surface.geometry_normal;
}

void fillViewStructure(out View view, float3 map_normal, float3 world_pos)
{
    view.PointToCameraNormalized = normalize(g_cameraPos - world_pos);
    view.NdotV = max(dot(map_normal, view.PointToCameraNormalized), 0.001f);
    view.reflectionDir = reflect(-view.PointToCameraNormalized, map_normal);
}


#endif