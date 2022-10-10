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
		Matr<4> viewInv;
		Vec3 BL;
		float padding0;
		Vec3 Right;
		float time_since_epoch;
		Vec3 Top;
		uint32_t sampleCount;
		Vec3 cameraPos;
		uint32_t pointLightNum;
		GpuPointLight gpuPointLight[LightSystem::MAX_POINT_LIGHTS];
		Matr<4> viewProjPointLight[LightSystem::MAX_POINT_LIGHTS][6];
		uint32_t directionalLightNum;
		float padding2[3];
		GpuDirectionalLight gpuDirectionalLight[LightSystem::MAX_DIRECTIONAL_LIGHTS];
		Matr<4> viewProjDirectionalLight[LightSystem::MAX_DIRECTIONAL_LIGHTS];

		PerFrameBuffer() {}
		PerFrameBuffer(const Camera& camera, const std::vector<PointLight>& pointLights, 
			const std::vector<GpuDirectionalLight> &directionalLights, const std::chrono::steady_clock::time_point& currentTime, uint32_t _sampleCount);
	};
}
