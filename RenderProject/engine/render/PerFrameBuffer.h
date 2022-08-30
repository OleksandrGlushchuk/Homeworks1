#pragma once
#include "LightSystem.h"
#include <vector>
class Camera;
namespace engine
{
	struct PerFrameBuffer
	{
		Matr<4> viewProj;
		Vec3 BL;
		float padding0;
		Vec3 Right;
		float padding1;
		Vec3 Top;
		float padding2;
		Vec3 cameraPos;
		uint32_t pointLightNum;

		GpuPointLight gpuPointLight[LightSystem::MAX_POINT_LIGHTS];

		PerFrameBuffer() {}
		PerFrameBuffer(const Camera& camera, const std::vector<PointLight>& pointLights);
	};
}
