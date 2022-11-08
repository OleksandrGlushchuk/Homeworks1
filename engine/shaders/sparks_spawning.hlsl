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

struct GS_INPUT
{
    float3 position : POSITION;
    float sphereVelocity : SPHERE_VELOCITY;
    float3 normal : NORMAL;
    float creationTime : CREATION_TIME;
    float3 spherePos : SPHERE_POS;
};


GS_INPUT vs_main(VS_INPUT input)
{
    GS_INPUT output;
    float4x4 TransformMatrix = float4x4(input.transform_x, input.transform_y, input.transform_z, input.transform_w);
    float4x4 WorldMatrix = mul(g_meshToModelMatrix, TransformMatrix);

    output.position = mul(float4(input.position, 1.0f), WorldMatrix).xyz;
    output.normal = mul(float4(input.normal, 0), WorldMatrix).xyz;
    output.creationTime = input.creationTime;
    output.spherePos = input.spherePos;
    output.sphereVelocity = input.sphereVelocity;

    return output;
}

float3 findCenter(float3 A, float3 B, float3 C)
{
    //float3 M1 = 0.5f * (C + A);
    //float3 M2 = 0.5f * (C + B);
    
    //float3 AM = M2 - A;
    //float3 BM = M1 - B;
    
    //return A + (B - A) / (AM - BM);
    
    return float3(A + B + C) / 3.f;
}

[maxvertexcount(1)]
void gs_main(triangle GS_INPUT input[3])
{
    float3 pos;
    pos = findCenter(input[0].position, input[1].position, input[2].position);
 
    
    float spawnTime = input[0].creationTime + length(pos.xyz - input[0].spherePos) / input[0].sphereVelocity;
    if (spawnTime > g_time - g_deltaTime && spawnTime < g_time)
    {
        uint prevCount;
        InterlockedAdd(particlesRange[1], 1, prevCount);
        
        Particle newParticle;
        newParticle.spawnTime = spawnTime;
        newParticle.velocity = input[0].normal * SPAWN_PARTICLE_SPEED;
        newParticle.position = pos.xyz + newParticle.velocity;
        newParticle.padding = 0;
   
        particlesData[(particlesRange[0] + prevCount) % g_sparksBufferSize] = newParticle;
    }
}