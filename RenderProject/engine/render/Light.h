#pragma once
#include "../math/vec3.h"
#include "../math/Matr.h"
#include "TransformSystem.h"
#include "EmissiveInstances.h"
#include "MeshSystem.h"
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
		PointLight(const Vec3& _irradiance, float distance, const Vec3& _position, const float _radius, const std::shared_ptr<Model>& model)
		{
			radius = _radius;
			radiance = irradianceAtDistanceToRadiance(_irradiance, distance, radius);

			Transform transform = Transform::Identity();
			transform.SetScale({ _radius,_radius,_radius });
			transform.SetPosition({ _position.e[0], _position.e[1], _position.e[2] });

			transformID = engine::TransformSystem::instance().m_transforms.insert(transform);
			
			engine::MeshSystem::instance().addInstance(model, EmissiveInstances::Instance(radiance, transformID));
		}
	};

	struct PointLightBuffer
	{
		Vec3 radiance;
		float padding0;
		Vec3 position;
		float radius;
		PointLightBuffer() {}
		PointLightBuffer(const PointLight& pointLight) :
			radiance(pointLight.radiance),
			position(engine::TransformSystem::instance().m_transforms[pointLight.transformID].getTransformMatrix()[3]),
			radius(pointLight.radius)
		{}
	};
}