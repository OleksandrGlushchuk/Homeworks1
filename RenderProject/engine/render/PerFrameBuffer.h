#pragma once
#include "../math/vec3.h"
#include "../math/Matr.h"
#include "Light.h"
#include <vector>
static const uint32_t MAX_POINT_LIGHTS = 8;
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

		PointLightBuffer pointLightBuffer[MAX_POINT_LIGHTS];

		PerFrameBuffer() {}
		PerFrameBuffer(const Matr<4>& _viewProj, const Vec3& _BL, const Vec3& _Right, const Vec3& _Top,
			const Vec3& _cameraPos, const std::vector<PointLight>& pointLights) :
			viewProj(_viewProj), BL(_BL), Right(_Right), Top(_Top), cameraPos(_cameraPos), pointLightNum(pointLights.size())
		{
			for (uint32_t i = 0; i < pointLightNum; ++i)
			{
				pointLightBuffer[i] = PointLightBuffer(pointLights[i]);
			}

		}
	};
}
