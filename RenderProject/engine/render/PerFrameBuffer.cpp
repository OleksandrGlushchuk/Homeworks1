#include "PerFrameBuffer.h"
#include "../math/camera.h"

namespace engine
{
	PerFrameBuffer::PerFrameBuffer(const Camera& camera, const std::vector<PointLight>& pointLights) :
		viewProj(camera.m_viewProj), BL(camera.BottomLeft), Right(camera.BR_M_BL), Top(camera.TL_M_BL),
		cameraPos(camera.position()), pointLightNum(pointLights.size())
	{
		for (uint32_t i = 0; i < pointLightNum; ++i)
		{
			gpuPointLight[i] = GpuPointLight(pointLights[i]);

			for (uint32_t j = 0; j < 6; ++j)
			{
				viewProjPointLight[i][j] = LightSystem::instance().getViewProjPointLight(i, j);
			}
		}

	}
}