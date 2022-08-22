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
	public:
		static const uint32_t MAX_POINT_LIGHTS = 8;
		static void init();
		static void deinit();
		static LightSystem& instance();

		void addPointLight(const Vec3& _irradiance, float distance, const Vec3& _position, const float _radius, const std::shared_ptr<Model>& model);
		const std::vector<PointLight>& getPointLights() const { return m_pointLights; }
	};
}