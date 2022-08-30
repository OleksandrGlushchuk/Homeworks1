#include "LightSystem.h"

namespace engine
{
	LightSystem* LightSystem::s_instance = nullptr;
	void LightSystem::init()
	{
		ALWAYS_ASSERT(s_instance == nullptr);
		s_instance = new LightSystem;
	}

	void LightSystem::deinit()
	{
		ALWAYS_ASSERT(s_instance != nullptr);
		delete s_instance;
		s_instance = nullptr;
	}

	LightSystem& LightSystem::instance()
	{
		ALWAYS_ASSERT(s_instance); return *s_instance;
	}

	void LightSystem::addPointLight(const Vec3& _irradiance, float distance, const Vec3& _position, const float _radius, const std::shared_ptr<Model>& model)
	{
		ALWAYS_ASSERT(m_pointLights.size() < MAX_POINT_LIGHTS, "cannot add more pointLights than MAX_POINT_LIGHTS." 
			"Change this constant in globals.hlsli and LightSystem.h");

		m_pointLights.emplace_back(_irradiance, distance, _position, _radius);
		Transform transform = Transform::Identity();
		transform.SetScale({ _radius,_radius,_radius });
		transform.SetPosition({ _position.e[0], _position.e[1], _position.e[2] });

		auto& pointLight = m_pointLights.back();
		pointLight.transformID = engine::TransformSystem::instance().m_transforms.insert(transform);
		engine::MeshSystem::instance().addInstance(model, EmissiveInstances::Instance(pointLight.radiance, pointLight.transformID));
	}
}