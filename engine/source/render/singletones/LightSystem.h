#pragma once
#include "../DxRes.hpp"
#include "../Light.h"
#include <vector>
#include "../../math/camera.h"

namespace engine
{
	class LightSystem
	{
		static LightSystem* s_instance;
		LightSystem() {}
		LightSystem(const LightSystem& other) = delete;
		LightSystem& operator=(const LightSystem& other) = delete;
		LightSystem(LightSystem&& other) noexcept = delete;
		LightSystem& operator=(LightSystem&& other) noexcept = delete;
		std::vector<PointLight> m_pointLights;
		std::vector<GpuDirectionalLight> m_directionalLights;
		Matr<4> m_pointLightProj = Matr<4>::identity();
		std::vector<Matr<4>> m_viewProjPointLight;

		Matr<4> m_directionalLightProj;
		std::vector<Matr<4>> m_viewDirectionalLight;
		std::vector<Matr<4>> m_viewProjDirectionalLight;
		Vec3 cameraCenter;
		float R;

		float texelSize;
	public:
		static const uint32_t MAX_POINT_LIGHTS = 5;
		static const uint32_t MAX_DIRECTIONAL_LIGHTS = 2;
		static void init();
		static void deinit();
		static LightSystem& instance();

		void addPointLight(const Vec3& _irradiance, float distance, const Vec3& _position, const float _radius, const std::shared_ptr<Model>& model);
		void addDirectionalLight(const Vec3& radiance, const Vec3& direction);
		void updatePointLightMatrices();
		void updateDirectionalLightMatrices(const Camera& camera);
		void setDirectionalLightFrustum(const Camera& camera);
		const Matr<4>& getViewProjPointLight(uint32_t pointLightIndex, uint32_t frustumIndex)
		{
			return m_viewProjPointLight[pointLightIndex * 6 + frustumIndex];
		}

		const Matr<4>& getViewProjDirectionalLight(uint32_t directionalLightIndex)
		{
			return m_viewProjDirectionalLight[directionalLightIndex];
		}
		const std::vector<PointLight>& getPointLights() const { return m_pointLights; }
		const std::vector<GpuDirectionalLight>& getDirectionalLights() const { return m_directionalLights; }
	};
}