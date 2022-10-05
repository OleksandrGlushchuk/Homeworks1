#include "PerFrameBuffer.h"
#include "../math/camera.h"
#include <chrono>

namespace engine
{
	PerFrameBuffer::PerFrameBuffer(const Camera& camera, const std::vector<PointLight>& pointLights, const std::vector<GpuDirectionalLight>& directionalLights, uint32_t _sampleCount) :
		viewProj(camera.m_viewProj), viewInv(camera.m_viewInv), BL(camera.BottomLeft), Right(camera.BR_M_BL), 
		time_since_epoch(std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now().time_since_epoch()).count()), 
		Top(camera.TL_M_BL), cameraPos(camera.position()), pointLightNum(pointLights.size()), 
		directionalLightNum(directionalLights.size()), sampleCount(_sampleCount)
	{
		for (uint32_t i = 0; i < pointLightNum; ++i)
		{
			gpuPointLight[i] = GpuPointLight(pointLights[i]);

			for (uint32_t j = 0; j < 6; ++j)
			{
				viewProjPointLight[i][j] = LightSystem::instance().getViewProjPointLight(i, j);
			}
		}

		for (uint32_t i = 0; i < directionalLightNum; ++i)
		{
			gpuDirectionalLight[i] = directionalLights[i];
			viewProjDirectionalLight[i] = LightSystem::instance().getViewProjDirectionalLight(i);
		}

	}
}