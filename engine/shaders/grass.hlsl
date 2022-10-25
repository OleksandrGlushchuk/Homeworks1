#include "globals.hlsli"
#include "gbuffer.hlsli"
#include "wind.hlsli"
static const uint TEXTURES_IN_ONE_BUSH = 5;
static const uint MAX_VERTEX_COUNT = TEXTURES_IN_ONE_BUSH * 3;

Texture2D<float3> g_colorMap : register(t0);
Texture2D<float3> g_normalMap : register(t1);
Texture2D<float> g_roughnessMap : register(t2);
Texture2D<float> g_metalnessMap : register(t3);
Texture2D<float> g_AOMap : register(t4);
Texture2D<float> g_opacityMap : register(t5);
Texture2D<float3> g_translucency : register(t6);


struct VS_INPUT
{
    float3 position : POSITION;
    float2 tex_coord : TEXCOORD0;
    float3 grass_pos : GRASS_POS;
};

struct GS_INPUT
{
    float3 grass_pos : GRASS_POS;
    float3 vertex_pos : VERTEX_POS;
    float2 tex_coord : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

struct PS_INPUT
{
    float4 position : SV_Position;
    float3 world_pos : WORLD_POS;
    float2 tex_coord : TEXCOORD0;
    float3 normal : PNORMAL;
    float3 tangent : PTANGENT;
    float3 bitangent : PBITANGENT;
};



GS_INPUT vs_main(VS_INPUT input, uint vertexID : SV_VertexID)
{
    GS_INPUT output;
    output.vertex_pos = input.position;
    output.tex_coord = input.tex_coord;
    output.grass_pos = input.grass_pos;
    output.normal = float3(0, 0, -1.f);
    output.tangent = float3(1.f, 0, 0);
    output.bitangent = float3(0, -1.f, 0);
    return output;
}

[maxvertexcount(MAX_VERTEX_COUNT)]
void gs_main(triangle GS_INPUT input[3], inout TriangleStream<PS_INPUT> outputStream)
{
    float bush_angle = M_PI / TEXTURES_IN_ONE_BUSH;

    float2 inst_pos = input[0].grass_pos.xz;
    float sw, cw;
    sw = WIND_DIR.y;
    cw = WIND_DIR.x;
    
    float3x3 windMatr = float3x3(float3(cw, 0, -sw), float3(0, 1, 0), float3(sw, 0, cw));
    float3x3 windInvMatr = transpose(windMatr);
    float wangle = computeGrassAngle(inst_pos, float2(1,0));
    wangle = abs(wangle) > 0.01f ? wangle : sign(wangle) * 0.01f;
    float R = (0.5f) / (wangle);
    
    
    [unroll]
    for (uint i = 0; i < TEXTURES_IN_ONE_BUSH; ++i)
    {
        float angle_cos, angle_sin;
        sincos(bush_angle * i, angle_sin, angle_cos);
        float3x3 rotate_y = float3x3(float3(angle_cos, 0, angle_sin), float3(0, 1, 0), float3(-angle_sin, 0, angle_cos));
        
        [unroll]
        for (uint vertex = 0; vertex < 3; ++vertex)
        {
            PS_INPUT output;
            float3 pos = input[vertex].vertex_pos;
            pos = mul(pos, rotate_y);
            output.normal = mul(input[vertex].normal, rotate_y);
            output.tangent = mul(input[vertex].tangent, rotate_y);
            output.bitangent = mul(input[vertex].bitangent, rotate_y);

            {
                sincos(wangle * pos.y/0.5f, sw, cw);
                float3x3 windRotationMatrix = float3x3(float3(cw, -sw, 0), float3(sw, cw, 0), float3(0, 0, 1));
                
                pos = mul(pos, windMatr);
                output.normal = mul(output.normal, windMatr);
                output.tangent = mul(output.tangent, windMatr);
                output.bitangent = mul(output.bitangent, windMatr);
                
                pos.x -= R;
                pos = mul(pos, windRotationMatrix);
                output.normal = mul(output.normal, windRotationMatrix);
                output.tangent = mul(output.tangent, windRotationMatrix);
                output.bitangent = mul(output.bitangent, windRotationMatrix);
                
                pos.x += R;
                pos = mul(pos, windInvMatr);
                output.normal = mul(output.normal, windInvMatr);
                output.tangent = mul(output.tangent, windInvMatr);
                output.bitangent = mul(output.bitangent, windInvMatr);
            }
           

            pos += input[vertex].grass_pos;
            output.world_pos = pos;
            output.position = mul(float4(pos, 1), g_viewProj);
            output.tex_coord = input[vertex].tex_coord;
           
            outputStream.Append(output);
        }
        outputStream.RestartStrip();
    }
}

cbuffer MaterialConstantBuffer : register(b1)
{
    float g_metalnessValue;
    float g_roughnessValue;
    bool g_hasMetalnessMap;
    bool g_hasRoughnessMap;
    
    bool g_hasNormalMap;
    bool g_reversedNormalTextureY;
    float2 paddingMCB;
}


float4 grassRGBA(float2 tc)
{
    float4 color;
    color.xyz = g_colorMap.Sample(g_maskedSamplerState, tc).xyz;
    color.a = g_opacityMap.Sample(g_maskedSamplerState, tc);

    const int FADING_PIXELS = 1;
    const float THRESHOLD = 0.1;
    float deltaAlpha = min(1.0, abs(ddx(color.a)) + abs(ddy(color.a)));
    float edgeDistance = (color.a - THRESHOLD) / deltaAlpha;
    color.a = saturate(edgeDistance / FADING_PIXELS);
	
    return color;
}

#include "opaque_helpers.hlsli"

PS_OUTPUT ps_main(PS_INPUT input, bool isFrontFace : SV_IsFrontFace)
{
    float4 grassColor = grassRGBA(input.tex_coord);
    if (grassColor.a < 1.f)
    {
        discard;
        PS_OUTPUT output;
        return output;
    }
    
    Surface surface;
    fillSurfaceStructure(surface, input.tex_coord, input.normal, 
    input.tangent, input.bitangent, g_translucency.Sample(g_samplerState,input.tex_coord), isFrontFace);
    
    surface.albedo = grassColor.xyz;
    surface.albedo *= g_AOMap.Sample(g_samplerState, input.tex_coord);
    
    PS_OUTPUT gbuffer = GBuffer(surface, 0);
    return gbuffer;
}

