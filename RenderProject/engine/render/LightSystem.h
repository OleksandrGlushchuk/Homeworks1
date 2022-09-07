#pragma once
#include "DxRes.hpp"
#include "Light.h"
#include <vector>

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
		Matr<4> m_proj;
		std::vector<Matr<4>> m_viewProjPointLight;

		void setPerspective(float fov, float aspect, float p_near, float p_far)
		{
			Matr<4>::fill_row(m_proj[0], { (1.f / tanf(fov / 2.f)) / aspect,	0,							0,									0 });
			Matr<4>::fill_row(m_proj[1], { 0,									1.f / tanf(fov / 2.f),		0,									0 });
			Matr<4>::fill_row(m_proj[2], { 0,									0,							p_near / (p_near - p_far),			1 });
			Matr<4>::fill_row(m_proj[3], { 0,									0,							-p_far * p_near / (p_near - p_far),	0 });
		}
	public:
		static const uint32_t MAX_POINT_LIGHTS = 5;
		static void init();
		static void deinit();
		static LightSystem& instance();

		void addPointLight(const Vec3& _irradiance, float distance, const Vec3& _position, const float _radius, const std::shared_ptr<Model>& model);
		void updatePointLightMatrices();
		const Matr<4>& getViewProjPointLight(uint32_t pointLightIndex, uint32_t frustumIndex)
		{
			return m_viewProjPointLight[pointLightIndex * 6 + frustumIndex];
		}
		const std::vector<PointLight>& getPointLights() const { return m_pointLights; }
	};
}