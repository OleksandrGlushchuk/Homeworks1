#pragma once
#include "singletones/LightSystem.h"
#include <vector>
#include <chrono>
class Camera;
namespace engine
{
	struct PerFrameBuffer
	{
		Matr<4> viewProj;
		Matr<4> viewProjInv;
		Matr<4> viewInv;
		Vec3 BL;
		uint32_t screenWidth;
		Vec3 Right;
		float time_since_epoch;
		Vec3 Top;
		uint32_t sampleCount;
		Vec3 cameraPos;
		uint32_t screenHeight;

		uint32_t pointLightNum;
		uint32_t directionalLightNum;
		float padding1[2];

		GpuPointLight gpuPointLight[LightSystem::MAX_POINT_LIGHTS];
		GpuDirectionalLight gpuDirectionalLight[LightSystem::MAX_DIRECTIONAL_LIGHTS];
		PerFrameBuffer() {}
		PerFrameBuffer(const Camera& camera, const std::chrono::steady_clock::time_point& currentTime, 
			const std::vector<PointLight>& pointLights,
			const std::vector<GpuDirectionalLight>& directionalLights,
			uint32_t _sampleCount, uint32_t _screenWidth, uint32_t _screenHeight);
	};
}
