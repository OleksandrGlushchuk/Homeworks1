#pragma once
#include "RenderTargetView.h"
#include "Shader.h"
#include "ConstantBuffer.h"
#include "../math/vec3.h"

namespace engine
{
	struct PostProcessBuffer
	{
		float m_EV100;
		float padding[3];
		PostProcessBuffer() {}
		PostProcessBuffer(float _EV100) : m_EV100(_EV100)
		{}
	};
	class PostProcess
	{
		
		Shader m_shader;
		ConstantBuffer<PostProcessBuffer> m_constantBuffer;
	public:
		float EV100;
		PostProcess() {}
		void Init(float _EV100)
		{
			EV100 = _EV100;
			m_shader.Init(L"engine/shaders/resolve.hlsl", nullptr, 0, ShaderEnabling(true, false));
			m_constantBuffer.Init(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		}
		void Resolve(RenderTargetView& dst) 
		{
			m_shader.Bind();
			engine::s_deviceContext->OMSetRenderTargets(1, dst.GetRTVPtrToPrt(), nullptr);
			m_constantBuffer.Update(PostProcessBuffer(EV100));
			m_constantBuffer.BindPS(1);
			engine::s_deviceContext->Draw(3, 0);
		}
	};
}