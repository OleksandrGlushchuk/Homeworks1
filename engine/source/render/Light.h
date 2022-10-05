#pragma once
#include "../math/vec3.h"
#include "../math/Matr.h"
#include "singletones/TransformSystem.h"
#include "EmissiveInstances.h"
#include "singletones/MeshSystem.h"
#include "ModelID.h"

namespace engine
{
	class PointLight
	{
		Vec3 irradianceAtDistanceToRadiance(Vec3 irradiance, float distance, float radius)
		{
			float angleSin = std::min(1.f, radius / distance);
			float angleCos = sqrtf(1.f - angleSin * angleSin);
			float occupation = 1.f - angleCos;
			return irradiance / occupation;
		}
	public:
		Vec3 radiance;
		float radius;
		uint32_t transformID;

		PointLight() {}
		PointLight(const Vec3& _irradiance, float distance, const Vec3& _position, const float _radius)
		{
			radius = _radius;
			radiance = irradianceAtDistanceToRadiance(_irradiance, distance, radius);
		}
	};

	struct GpuPointLight
	{
		Vec3 radiance;
		float padding0;
		Vec3 position;
		float radius;
		GpuPointLight() {}
		GpuPointLight(const PointLight& pointLight) :
			radiance(pointLight.radiance),
			position(engine::TransformSystem::instance().m_transforms[pointLight.transformID].getTransformMatrix()[3]),
			radius(pointLight.radius)
		{}
	};

	struct GpuDirectionalLight
	{
		Vec3 radiance;
		float padding0;
		Vec3 direction;
		float padding1;
		GpuDirectionalLight() {}
		GpuDirectionalLight(const Vec3& _radiance, const Vec3& _direction) : radiance(_radiance), direction(_direction) {}
	};
}