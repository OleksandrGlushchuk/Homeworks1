#include "LightSystem.h"
#include "ShadowManager.h"
#include "../../math/MatrixHelpers.h"

namespace engine
{
	LightSystem* LightSystem::s_instance = nullptr;
	const float p_near = 0.01f, p_far = 1000.f, fovy = M_PI / 2.f;
	void LightSystem::init()
	{
		ALWAYS_ASSERT(s_instance == nullptr);
		s_instance = new LightSystem;
		s_instance->m_pointLightClipSphere = ModelManager::instance().GetUnitSphereModel();
		math::setPerspective(fovy, 1.f, p_near, p_far, s_instance->m_pointLightProj);
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

	void LightSystem::updatePointLightInstanceBuffer()
	{
		uint32_t size = m_pointLights.size();
		
		m_pointLightInstanceBuffer.Init(size, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		auto mapping = m_pointLightInstanceBuffer.Map();
		PointLightInstanceBuffer* dst = (PointLightInstanceBuffer*)mapping.pData;
		for (uint32_t i = 0; i < size; ++i)
		{
			std::copy(m_viewProjPointLight.begin() + i * 6, m_viewProjPointLight.begin() + (i + 1) * 6, dst[i].viewProj);
		}
		m_pointLightInstanceBuffer.Unmap();
	}

	void LightSystem::updateDirectionalLightInstanceBuffer()
	{
		uint32_t size = m_directionalLights.size();

		m_directionalLightInstanceBuffer.Init(size, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		auto mapping = m_directionalLightInstanceBuffer.Map();
		DirectionalLightInstanceBuffer* dst = (DirectionalLightInstanceBuffer*)mapping.pData;
		for (uint32_t i = 0; i < size; ++i)
		{
			dst[i].viewProj = m_viewProjDirectionalLight[i];
		}
		m_directionalLightInstanceBuffer.Unmap();
	}

	void LightSystem::addDirectionalLight(const Vec3& radiance, const Vec3& direction)
	{
		ALWAYS_ASSERT(m_directionalLights.size() < MAX_DIRECTIONAL_LIGHTS, "cannot add more directionalLights than MAX_DIRECTIONAL_LIGHTS."
			"Change this constant in globals.hlsli and LightSystem.h");

		m_directionalLights.emplace_back(radiance, direction.normalized());
		m_viewProjDirectionalLight.push_back(Matr<4>::identity());
		m_viewDirectionalLight.push_back(Matr<4>::identity());
	}

	void LightSystem::updatePointLightMatrices()
	{
		Matr<4> right({
			{ 0,0,1,0 },
			{ 0,1,0,0 },
			{ -1,0,0,0 },
			{ 0,0,0,0 }
			});
		Vec3 light_right_x(0, 0, -1), light_right_y(0, 1, 0), light_right_z(1, 0, 0);

		Matr<4> left({
			{ 0,0,-1,0 },
			{ 0,1,0,0 },
			{ 1,0,0,0 },
			{ 0,0,0,0 }
			});
		Vec3 light_left_x(0, 0, 1), light_left_y(0, 1, 0), light_left_z(-1, 0, 0);

		Matr<4> up({
			{ 1,0,0,0 },
			{ 0,0,1,0 },
			{ 0,-1,0,0 },
			{ 0,0,0,0 }
			});
		Vec3 light_up_x(1, 0, 0), light_up_y(0, 0, -1), light_up_z(0, 1, 0);

		Matr<4> down({
			{ 1,0,0,0 },
			{ 0,0,-1,0 },
			{ 0,1,0,0 },
			{ 0,0,0,0 }
			});
		Vec3 light_down_x(1, 0, 0), light_down_y(0, 0, 1), light_down_z(0, -1, 0);

		Matr<4> front({
			{ 1,0,0,0 },
			{ 0,1,0,0 },
			{ 0,0,1,0 },
			{ 0,0,0,0 }
			});
		Vec3 light_front_x(1, 0, 0), light_front_y(0, 1, 0), light_front_z(0, 0, 1);

		Matr<4> back({
			{ -1,0,0,0 },
			{ 0,1,0,0 },
			{ 0,0,-1,0 },
			{ 0,0,0,0 }
			});
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
			
			m_viewProjPointLight[pointLightFirstFrustum] = right * m_pointLightProj;
			m_viewProjPointLight[pointLightFirstFrustum + 1] = left * m_pointLightProj;
			m_viewProjPointLight[pointLightFirstFrustum + 2] = up * m_pointLightProj;
			m_viewProjPointLight[pointLightFirstFrustum + 3] = down * m_pointLightProj;
			m_viewProjPointLight[pointLightFirstFrustum + 4] = front * m_pointLightProj;
			m_viewProjPointLight[pointLightFirstFrustum + 5] = back * m_pointLightProj;
		}
	}

	void LightSystem::updateDirectionalLightMatrices(const Camera& camera)
	{
		uint32_t directionalLightSize = m_directionalLights.size();
		if (directionalLightSize == 0)
			return;

		const Vec3 cameraPos = camera.position();
		cameraCenter = cameraPos + camera.forward() * (0.5f * ShadowManager::instance().GetDirectionalLightShadowDistance());
		for (uint32_t i = 0; i < directionalLightSize; ++i)
		{
			Vec3 newPos = cameraCenter;
			newPos.mult(m_viewDirectionalLight[i], 0);
			newPos.e[0] = texelSize * int(newPos.e[0] / texelSize);
			newPos.e[1] = texelSize * int(newPos.e[1] / texelSize);
			newPos.mult(m_viewDirectionalLight[i].invert(), 0);

			math::setView(newPos, -m_directionalLights[i].direction, m_viewDirectionalLight[i]);
			m_viewProjDirectionalLight[i] = m_viewDirectionalLight[i] * m_directionalLightProj;
		}
	}

	void LightSystem::setDirectionalLightFrustum(const Camera& camera)
	{
		const Vec3 cameraPos = camera.position();
		Vec3 topLeftDir = (camera.TopLeft - cameraPos).normalized();
		float diagCos = Vec3::dot(topLeftDir, camera.forward());
		float hypotenuse = ShadowManager::instance().GetDirectionalLightShadowDistance() / diagCos;
		cameraCenter = cameraPos + camera.forward() * (0.5f * ShadowManager::instance().GetDirectionalLightShadowDistance());
		R = (hypotenuse * topLeftDir - cameraCenter).length();
		texelSize = 2.f * R / ShadowManager::instance().GetDirectionalLightDSResolution();
		math::setOrtho(-R, R, -R, R, -p_far*10, p_far*10, m_directionalLightProj);
	}
}