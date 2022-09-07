#include "LightSystem.h"

namespace engine
{
	LightSystem* LightSystem::s_instance = nullptr;
	const float p_near = 0.01f, p_far = 10000.f, fovy = M_PI / 2.f;
	void LightSystem::init()
	{
		ALWAYS_ASSERT(s_instance == nullptr);
		s_instance = new LightSystem;
		s_instance->setPerspective(fovy, 1.f, p_near, p_far);
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

		m_viewProjPointLight.resize(m_viewProjPointLight.size() + 6);
	}

	void LightSystem::updatePointLightMatrices()
	{
		Matr<4> right(
			{ 0,0,1,0 },
			{ 0,1,0,0 },
			{ -1,0,0,0 },
			{ 0,0,0,0 }
		);
		Vec3 light_right_x(0, 0, -1), light_right_y(0, 1, 0), light_right_z(1, 0, 0);

		Matr<4> left(
			{ 0,0,-1,0 },
			{ 0,1,0,0 },
			{ 1,0,0,0 },
			{ 0,0,0,0 }
		);
		Vec3 light_left_x(0, 0, 1), light_left_y(0, 1, 0), light_left_z(-1, 0, 0);

		Matr<4> up(
			{ 1,0,0,0 },
			{ 0,0,1,0 },
			{ 0,-1,0,0 },
			{ 0,0,0,0 }
		);
		Vec3 light_up_x(1, 0, 0), light_up_y(0, 0, -1), light_up_z(0, 1, 0);

		Matr<4> down(
			{ 1,0,0,0 },
			{ 0,0,-1,0 },
			{ 0,1,0,0 },
			{ 0,0,0,0 }
		);
		Vec3 light_down_x(1, 0, 0), light_down_y(0, 0, 1), light_down_z(0, -1, 0);

		Matr<4> front(
			{ 1,0,0,0 },
			{ 0,1,0,0 },
			{ 0,0,1,0 },
			{ 0,0,0,0 }
		);
		Vec3 light_front_x(1, 0, 0), light_front_y(0, 1, 0), light_front_z(0, 0, 1);

		Matr<4> back(
			{ -1,0,0,0 },
			{ 0,1,0,0 },
			{ 0,0,-1,0 },
			{ 0,0,0,0 }
		);
		Vec3 light_back_x(-1, 0, 0), light_back_y(0, 1, 0), light_back_z(0, 0, -1);

		uint32_t pointLightSize = m_pointLights.size();
		uint32_t pointLightFirstFrustum;
		for (uint32_t i = 0; i < pointLightSize; ++i)
		{
			pointLightFirstFrustum = i * 6;
			const Vec3& position = TransformSystem::instance().m_transforms[m_pointLights[i].transformID].getPosition();

			Matr<4>::fill_row(right[3], { -Vec3::dot(position, light_right_x),	-Vec3::dot(position, light_right_y), -Vec3::dot(position, light_right_z), 1 });
			Matr<4>::fill_row(left[3],	{ -Vec3::dot(position, light_left_x),	-Vec3::dot(position, light_left_y),	 -Vec3::dot(position, light_left_z), 1 });
			Matr<4>::fill_row(up[3],	{ -Vec3::dot(position, light_up_x),		-Vec3::dot(position, light_up_y),	 -Vec3::dot(position, light_up_z), 1 });
			Matr<4>::fill_row(down[3],	{ -Vec3::dot(position, light_down_x),	-Vec3::dot(position, light_down_y),  -Vec3::dot(position, light_down_z), 1 });
			Matr<4>::fill_row(front[3], { -Vec3::dot(position, light_front_x),	-Vec3::dot(position, light_front_y), -Vec3::dot(position, light_front_z), 1 });
			Matr<4>::fill_row(back[3],	{ -Vec3::dot(position, light_back_x),	-Vec3::dot(position, light_back_y),  -Vec3::dot(position, light_back_z), 1 });
			
			m_viewProjPointLight[pointLightFirstFrustum] = right * m_proj;
			m_viewProjPointLight[pointLightFirstFrustum + 1] = left * m_proj;
			m_viewProjPointLight[pointLightFirstFrustum + 2] = up * m_proj;
			m_viewProjPointLight[pointLightFirstFrustum + 3] = down * m_proj;
			m_viewProjPointLight[pointLightFirstFrustum + 4] = front * m_proj;
			m_viewProjPointLight[pointLightFirstFrustum + 5] = back * m_proj;
		}
	}
}