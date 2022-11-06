#include "globals.hlsli"

struct Particle
{
    float3 position;
    float spawnTime;
    float3 velocity;
    float padding;
};

RWStructuredBuffer<Particle> particlesData : register(u0);
RWBuffer<uint> particlesRange : register(u1);
static const uint BUFFER_SIZE = 128;
static const float SPAWN_PARTICLE_SPEED = 2.2f;

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

void vs_main(VS_INPUT input)
{
    float4x4 TransformMatrix = float4x4(input.transform_x, input.transform_y, input.transform_z, input.transform_w);

    float4x4 WorldMatrix = mul(g_meshToModelMatrix, TransformMatrix);
    float4 pos = mul(float4(input.position, 1.0f), WorldMatrix);
    float3 normal = mul(float4(input.normal, 0), WorldMatrix).xyz;
    
    float spawnTime = input.creationTime + length(pos.xyz - input.spherePos) / input.sphereVelocity;
    if(spawnTime > g_time - g_deltaTime && spawnTime < g_time)
    {
        uint prevCount;
        InterlockedAdd(particlesRange[1], 1, prevCount);
        
        Particle newParticle;
        newParticle.spawnTime = spawnTime;
        newParticle.velocity = normal * SPAWN_PARTICLE_SPEED;
        newParticle.position = pos.xyz + newParticle.velocity;
        newParticle.padding = 0;
   
        particlesData[(particlesRange[0] + prevCount) % BUFFER_SIZE] = newParticle;
    }
}