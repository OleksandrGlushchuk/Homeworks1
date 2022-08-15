#pragma once
#include "RenderTarget.h"
#include "Shader.h"
#include "ConstantBuffer.h"
#include "../math/vec3.h"

namespace engine
{
	struct PostProcessBuffer
	{
		float m_EV100;
		Vec3 padding;
		PostProcessBuffer() {}
		PostProcessBuffer(float _EV100)
		{
			m_EV100 = _EV100;
		}
	};
	class PostProcess
	{
		
		Shader m_shader;
		ConstantBuffer<PostProcessBuffer> m_constantBuffer;
	public:
		float EV100=2.f;
		PostProcess() {}
		void Init()
		{
			m_shader.Init(L"source/shaders/postProcess.hlsl", nullptr, 0);
			m_constantBuffer.Init(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		}
		void Resolve(RenderTarget& dst, RenderTarget &src) 
		{
			m_shader.Bind();
			dst.Bind(src.GetDepthStencil());
			auto mapping = m_constantBuffer.Map();
			*(PostProcessBuffer*)mapping.pData = PostProcessBuffer(EV100);
			m_constantBuffer.Unmap();
			
			m_constantBuffer.BindPS(1);
			engine::s_deviceContext->Draw(3, 0);
		}
	};
}