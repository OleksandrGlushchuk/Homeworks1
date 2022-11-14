#pragma once
#include "../GrassField.h"

namespace engine
{
	class VegetationSystem
	{
		static VegetationSystem* s_instance;
		VegetationSystem() {}
		VegetationSystem(const VegetationSystem& other) = delete;
		VegetationSystem& operator=(const VegetationSystem& other) = delete;
		VegetationSystem(VegetationSystem&& other) noexcept = delete;
		VegetationSystem& operator=(VegetationSystem&& other) noexcept = delete;
		GrassField grassField;
	public:
		static void init();
		static void deinit();
		static VegetationSystem& instance();

		void renderSceneDepthToCubemaps()
		{
			grassField.renderSceneDepthToCubemaps();
		}

		void renderSceneDepthForDirectionalLights()
		{
			grassField.renderSceneDepthForDirectionalLights();
		}

		void render()
		{
			grassField.render();
		}

		void AddGrassArea(float width, float height, float r, const Vec3 &world_pos)
		{
			grassField.AddGrassArea(width, height, r, world_pos);
		}

		uint32_t Size()
		{
			return grassField.NumInstances();
		}
	};
}