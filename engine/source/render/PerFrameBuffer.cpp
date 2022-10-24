#include "PerFrameBuffer.h"
#include "../math/camera.h"
#include <chrono>

namespace engine
{
	PerFrameBuffer::PerFrameBuffer(const Camera& camera, const std::chrono::steady_clock::time_point& currentTime, 
		const std::vector<PointLight>& pointLights,
		const std::vector<GpuDirectionalLight>& directionalLights,
		uint32_t _sampleCount, uint32_t _screenWidth, uint32_t _screenHeight) :
		viewProj(camera.m_viewProj), viewProjInv(camera.m_viewProjInv), viewInv(camera.m_viewInv), BL(camera.BottomLeft), Right(camera.BR_M_BL), 
		time_since_epoch(std::chrono::duration_cast<std::chrono::duration<float>>(currentTime.time_since_epoch()).count()),
		Top(camera.TL_M_BL), cameraPos(camera.position()), sampleCount(_sampleCount), screenWidth(_screenWidth), screenHeight(_screenHeight),
		pointLightNum(pointLights.size()),
		directionalLightNum(directionalLights.size())
	{
		for (uint32_t i = 0; i < pointLightNum; ++i)
		{
			gpuPointLight[i] = GpuPointLight(pointLights[i]);
		}

		for (uint32_t i = 0; i < directionalLightNum; ++i)
		{
			gpuDirectionalLight[i] = directionalLights[i];
		}
	}
}