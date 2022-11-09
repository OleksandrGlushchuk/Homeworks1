#include "globals.hlsli"

cbuffer SparksBufferSize : register(b2)
{
    uint g_sparksBufferSize;
    float3 paddingSBS;
};

struct Particle
{
    float3 position;
    float spawnTime;
    float3 velocity;
    float padding;
};

RWStructuredBuffer<Particle> particlesData : register(u1);
RWBuffer<uint> particlesRange : register(u2);
static const float SPAWN_PARTICLE_SPEED = 0.04f;

cbuffer MeshToModel : register(b1)
{
    float4x4 g_meshToModelMatrix;
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

struct HS_INPUT
{
    float3 position : POSITION;
    float sphereVelocity : SPHERE_VELOCITY;
    float3 normal : NORMAL;
    float creationTime : CREATION_TIME;
    float3 spherePos : SPHERE_POS;
    
    float4x4 worldMatrix : WORLD_MATRIX;
};

struct HS_OUTPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct HS_COSTANT_PATCH_OUTPUT
{
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
    float creationTime : CREATION_TIME;
    float3 spherePos : SPHERE_POS;
    float sphereVelocity : SPHERE_VELOCITY;
    float4x4 worldMatrix : WORLD_MATRIX;
};

struct GS_INPUT
{
    float3 position : POSITION;
    float sphereVelocity : SPHERE_VELOCITY;
    float3 normal : NORMAL;
    float creationTime : CREATION_TIME;
    float3 spherePos : SPHERE_POS;
};

struct GS_OUTPUT
{
    float4 position : SV_Position;
};

static const float TES_FACTOR = 5.f;

HS_INPUT vs_main(VS_INPUT input)
{
    HS_INPUT output;
    float4x4 TransformMatrix = float4x4(input.transform_x, input.transform_y, input.transform_z, input.transform_w);
    float4x4 WorldMatrix = mul(g_meshToModelMatrix, TransformMatrix);

    output.position = input.position; //mul(float4(input.position, 1.0f), WorldMatrix).xyz;
    output.normal = input.position; //mul(float4(input.normal, 0), WorldMatrix).xyz;
    output.creationTime = input.creationTime;
    output.spherePos = input.spherePos;
    output.sphereVelocity = input.sphereVelocity;
    output.worldMatrix = WorldMatrix;
    
    
    return output;
}

HS_COSTANT_PATCH_OUTPUT hs_constant_patch_main(InputPatch<HS_INPUT, 3> inputPatch, uint patchID : SV_PrimitiveID)
{
    HS_COSTANT_PATCH_OUTPUT output;
    output.edges[0] = TES_FACTOR;
    output.edges[1] = TES_FACTOR;
    output.edges[2] = TES_FACTOR;
    
    output.inside = TES_FACTOR;
    
    output.creationTime = inputPatch[patchID].creationTime;
    output.worldMatrix = inputPatch[patchID].worldMatrix;
    output.spherePos = inputPatch[patchID].spherePos;
    output.sphereVelocity = inputPatch[patchID].sphereVelocity;
    
    return output;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("hs_constant_patch_main")]
HS_OUTPUT hs_main(InputPatch<HS_INPUT, 3> patch, uint pointID : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
    HS_OUTPUT output;
    output.normal = patch[pointID].normal;
    output.position = patch[pointID].position;

    return output;
}

[domain("tri")]
GS_INPUT ds_main(HS_COSTANT_PATCH_OUTPUT input, float3 uvwCoord : SV_DomainLocation, const OutputPatch<HS_OUTPUT, 3> patch)
{
    GS_INPUT output;
    output.position = uvwCoord.x * patch[0].position + uvwCoord.y * patch[1].position + uvwCoord.z * patch[2].position;
    output.position = /*output.position;*/ mul(float4(output.position, 1), input.worldMatrix).xyz;
    output.normal = /*patch[0].normal;*/ mul(float4(patch[0].normal, 0), input.worldMatrix).xyz;
    output.creationTime = input.creationTime;
    output.spherePos = input.spherePos;
    output.sphereVelocity = input.sphereVelocity;
    
    return output;
}



float3 findCenter(float3 A, float3 B, float3 C)
{
    return float3(A + B + C) / 3.f;
}
[maxvertexcount(1)]
void gs_main(triangle GS_INPUT input[3], inout PointStream<GS_OUTPUT> pstream)
{
    float3 pos;
    pos = findCenter(input[0].position, input[1].position, input[2].position);
    float3 normal = normalize(cross(input[1].position - input[0].position, input[2].position - input[0].position));
    
    float spawnTime = input[0].creationTime + length(pos.xyz - input[0].spherePos) / input[0].sphereVelocity;
    if (spawnTime > g_time - g_deltaTime && spawnTime < g_time + g_deltaTime)
    {
        uint prevCount;
        InterlockedAdd(particlesRange[1], 1, prevCount);
        
        Particle newParticle;
        newParticle.spawnTime = spawnTime;
        newParticle.velocity = normal * SPAWN_PARTICLE_SPEED;
        newParticle.position = pos.xyz + newParticle.velocity;
        newParticle.padding = 0;
   
        particlesData[(particlesRange[0] + prevCount) % g_sparksBufferSize] = newParticle;
    }
    //for (uint i = 0; i < 3; ++i)
    //{
    //    float3 pos;
    //    pos = input[i].position;
    //    float3 normal = input[i].normal;
    //    float spawnTime = input[0].creationTime + length(pos.xyz - input[0].spherePos) / input[0].sphereVelocity;
    //    if (spawnTime > g_time - g_deltaTime && spawnTime < g_time + g_deltaTime)
    //    {
    //        uint prevCount;
    //        InterlockedAdd(particlesRange[1], 1, prevCount);
        
    //        Particle newParticle;
    //        newParticle.spawnTime = spawnTime;
    //        newParticle.velocity = normal * SPAWN_PARTICLE_SPEED;
    //        newParticle.position = pos.xyz + newParticle.velocity;
    //        newParticle.padding = 0;
   
    //        particlesData[(particlesRange[0] + prevCount) % g_sparksBufferSize] = newParticle;
    //    }
    //}
    GS_OUTPUT output;
    output.position = 0;
    pstream.Append(output);
}