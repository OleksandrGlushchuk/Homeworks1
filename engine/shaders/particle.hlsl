#include "globals.hlsli"
#include "pbr_helpers.hlsli"
Texture2D<float4> g_smokeEMVA : register(t0);
Texture2D<float3> g_smokeRLT : register(t1);
Texture2D<float3> g_smokeBotBF : register(t2);

Texture2D<float> g_depthTexture : register(t3);
Texture2DMS<float> g_depthTextureMS : register(t4);

cbuffer ParticleConstantBuffer : register(b1)
{
    uint g_colsNum;
    uint g_rowsNum;
    float2 paddingPCB;
};

struct VS_INPUT
{
    float3 vertex_pos : VERTEX_POS;
    
    float3 particle_pos : PARTICLE_POS;
    float2 size : PARTICLE_SIZE;
    float4 tint : TINT;
    float angle : ANGLE;
    float thickness : THICKNESS;
    float creationTime : CREATION_TIME;
    float lifeTime : LIFE_TIME;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 world_pos : WORLD_POS;
    float2 tex_coord : TEXCOORD0;
    float2 tex_coord_next : TEXCOORD1;
    float4 tint : TINT;
    float frameFracTime : FRAME_FRAC_TIME;
    float3x3 rotation_z : ROTATION_Z;
    float thickness : THICKNESS;
};

void GetTexCoordsInAtlas(float creationTime, float lifeTime, uint vertexID, out float frameFracTime, out float2 currentTexCoord, out float2 nextTexCoord)
{
    float xStep = 1.f / g_colsNum;
    float yStep = 1.f / g_rowsNum;
    
    float currentLifeDuration = g_timeSinceEpoch - creationTime;
    float frameDuration = lifeTime / (g_rowsNum * g_colsNum);
    
    uint frameIndex = currentLifeDuration / frameDuration;
    
    frameFracTime = (currentLifeDuration - frameIndex * frameDuration) / frameDuration;
    
    uint rowIndex = float(frameIndex) / float(g_colsNum);
    uint colIndex = frameIndex % g_colsNum;
    
    uint nextFrameIndex = clamp(frameIndex + 1, 0, g_colsNum * g_rowsNum-1);
    uint nextRowIndex = float(nextFrameIndex) / float(g_colsNum);
    uint nextColIndex = (nextFrameIndex) % g_colsNum;
    
    
    if(vertexID == 0)
    {
        currentTexCoord = float2(colIndex * xStep, (1 + rowIndex) * yStep);
        nextTexCoord = float2(nextColIndex * xStep, (1 + nextRowIndex) * yStep);

    }
    else if(vertexID == 1)
    {
        currentTexCoord = float2(colIndex * xStep, rowIndex * yStep);
        nextTexCoord = float2(nextColIndex * xStep, nextRowIndex * yStep);
    }  
    else if(vertexID == 2)
    {
        currentTexCoord = float2((1 + colIndex) * xStep, rowIndex * yStep);
        nextTexCoord = float2((1 + nextColIndex) * xStep, nextRowIndex * yStep);
    }   
    else
    {
        currentTexCoord = float2((1 + colIndex) * xStep, (1 + rowIndex) * yStep);
        nextTexCoord = float2((1 + nextColIndex) * xStep, (1 + nextRowIndex) * yStep);
    }
}

PS_INPUT vs_main(VS_INPUT input, uint vertexID : SV_VertexID)
{
    PS_INPUT output;
    
    float3 vertex_pos = input.vertex_pos;
    vertex_pos.xy *= input.size;
    
    float cos_z, sin_z;
    sincos(input.angle, sin_z, cos_z);
    float3x3 rotation_z = float3x3(float3(cos_z, -sin_z, 0), float3(sin_z, cos_z, 0), float3(0, 0, 1));
    
    vertex_pos = mul(vertex_pos, rotation_z);
    
    vertex_pos = mul(float4(vertex_pos, 0), g_viewInv);
   
    vertex_pos += input.particle_pos;
    
    output.world_pos = vertex_pos;
    
    output.position = mul(float4(vertex_pos, 1), g_viewProj);
    
    GetTexCoordsInAtlas(input.creationTime, input.lifeTime, vertexID, output.frameFracTime, output.tex_coord, output.tex_coord_next);
    output.tint = input.tint;
    output.rotation_z = rotation_z;
    output.thickness = input.thickness;
    return output;
}

float4 ps_main(PS_INPUT input) : SV_Target
{
    float4 color = float4(0,0,0,0); 
    float3 rlt;
    float3 botBF; 
    
    
        const float g_mvScale = 0.001;

        float2 mvA = g_smokeEMVA.Sample(g_samplerState, input.tex_coord).gb;
        float2 mvB = g_smokeEMVA.Sample(g_samplerState, input.tex_coord_next).gb;
    
        mvA = 2 * mvA - 1;
        mvB = 2 * mvB - 1;

        float2 uvA = input.tex_coord;
        uvA -= mvA * g_mvScale * input.frameFracTime;

        float2 uvB = input.tex_coord_next;
        uvB -= mvB * g_mvScale * (input.frameFracTime - 1.f);

        float4 emva_a = g_smokeEMVA.Sample(g_samplerState, uvA);
        float4 emva_b = g_smokeEMVA.Sample(g_samplerState, uvB);

        float4 emva = lerp(emva_a, emva_b, input.frameFracTime);
        
        float3 rlt_a = g_smokeRLT.Sample(g_samplerState, uvA);
        float3 rlt_b = g_smokeRLT.Sample(g_samplerState, uvB);
        
        rlt = lerp(rlt_a, rlt_b, input.frameFracTime);
        
        float3 botBF_a = g_smokeBotBF.Sample(g_samplerState, uvA);
        float3 botBF_b = g_smokeBotBF.Sample(g_samplerState, uvB);
        
        botBF = lerp(botBF_a, botBF_b, input.frameFracTime);
        
        color.w = emva.w;
    
        
    
    float3 toLightDir;
    float3 lightRadiance;
    
    float lightAngleSin, cosHalfAngularDiameter, distanceToLight;
    float solid_angle;
    float shadowFactor;
    for (uint i = 0; i < g_pointLightNum; ++i)
    {
        toLightDir = g_pointLight[i].position - input.world_pos;
        distanceToLight = length(toLightDir);
        distanceToLight = max(distanceToLight, g_pointLight[i].radius);
        
        solid_angle = FindSolidAngle(distanceToLight, g_pointLight[i].radius, lightAngleSin, cosHalfAngularDiameter);
        
        lightRadiance = g_pointLight[i].radiance * solid_angle / HEMISPHERE_SOLID_ANGLE;
        toLightDir = mul(toLightDir, input.rotation_z);
        color.xyz += (toLightDir.x > 0) ? rlt.r * lightRadiance : rlt.g * lightRadiance;
        color.xyz += (toLightDir.y > 0) ? rlt.b * lightRadiance : botBF.r * lightRadiance;
        color.xyz += (toLightDir.z > 0) ? botBF.g * lightRadiance : botBF.b * lightRadiance;
    }
    for (uint j = 0; j < g_directionalLightNum; ++j)
    {
        toLightDir = g_directionalLight[j].direction;
        lightRadiance = g_directionalLight[j].radiance;
        
        toLightDir = mul(toLightDir, input.rotation_z);
        color.xyz += (toLightDir.x > 0) ? rlt.r * lightRadiance : rlt.g * lightRadiance;
        color.xyz += (toLightDir.y > 0) ? rlt.b * lightRadiance : botBF.r * lightRadiance;
        color.xyz += (toLightDir.z > 0) ? botBF.g * lightRadiance : botBF.b * lightRadiance;
    }
    
    float alphaFading;
    
    float sceneDepth;
    if(g_sampleCount == 1)
        sceneDepth = g_depthTexture.Load(float3(input.position.xy, 0));
    else
    {
        sceneDepth = g_depthTextureMS.Load(input.position.xy, 0);
        for (uint i = 1; i < g_sampleCount; ++i)
        {
            sceneDepth = min(sceneDepth, g_depthTextureMS.Load(input.position.xy, 1));
        }
    }
    
    
    float particleDepth = input.position.z;
    alphaFading = saturate((particleDepth - sceneDepth) / (input.thickness / length(g_cameraPos - input.world_pos)));
    color.a *= alphaFading;
    color *= input.tint;
    return color;
}