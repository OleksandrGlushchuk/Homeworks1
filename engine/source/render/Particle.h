#pragma once
#include "../math/vec4.h"
#include "../math/vec3.h"
#include "../math/vec2.h"

namespace engine
{
	class Particle
	{
	public:
		Particle() {}
		Particle(const Vec3& _position, const Vec2& _size, float _angle, const Vec4& _tint, 
			const float _creationTime, float _lifeTime) :
			position(_position), size(_size), thickness(0), angle(_angle), tint(_tint), 
			creationTime(_creationTime), lifeTime(_lifeTime)
		{}

		Vec3 position;
		Vec2 size;
		float thickness;
		float angle;
		Vec4 tint;
		float creationTime;
		float lifeTime;
	};

	struct GpuParticle
	{
		GpuParticle() {}
		GpuParticle(const Particle& particle) : position(particle.position), size(particle.size), tint(particle.tint), 
			angle(particle.angle), thickness(particle.thickness), 
			creationTime(particle.creationTime), lifeTime(particle.lifeTime)
		{}
		Vec3 position;
		Vec2 size;
		Vec4 tint;
		float angle;
		float thickness;
		float creationTime;
		float lifeTime;
	};
}