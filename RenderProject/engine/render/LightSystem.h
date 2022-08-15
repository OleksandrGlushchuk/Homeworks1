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
	public:
		static void init();
		static void deinit();
		static LightSystem& instance();
		std::vector<PointLight> pointLight;
	};
}