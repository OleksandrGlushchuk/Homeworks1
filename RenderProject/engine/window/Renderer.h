﻿#pragma once
#include "../math/camera.h"
#include "../math/ray.h"

#include "../objects/Sky.h"
#include "../render/DepthStencil.h"
#include "../render/RenderTarget.h"
#include "../render/PostProcess.h"

namespace engine::windows
{
	class Renderer
	{
		Camera camera;
		Sky m_sky;

		RenderTarget m_hdrRenderTarget;
		DxResPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	public:
		void Init(UINT width, UINT height);
		void Draw();

		void Render(RenderTarget& windowRenderTarget, const Camera& camera, PostProcess &postProcess);
		friend class Controller;
	};
}